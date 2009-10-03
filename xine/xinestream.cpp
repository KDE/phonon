/*  This file is part of the KDE project
    Copyright (C) 2006-2008 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2008      Ian Monroe <imonroe@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "xinestream.h"

#include <QMutexLocker>
#include <QtCore/QTextCodec>
#include <QEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QVarLengthArray>
#include <QUrl>

#include "backend.h"
#include "bytestream.h"
#include "events.h"
#include "mediaobject.h"
#include "videowidget.h"
#include "xineengine.h"
#include "xinethread.h"

extern "C" {
#define this _this_xine_
#include <xine/xine_internal.h>
#undef this
}

//#define DISABLE_FILE_MRLS

//#define streamClock(stream) stream->clock
#define streamClock(stream) stream->xine->clock

namespace Phonon
{
namespace Xine
{

void XineStream::xineEventListener(void *p, const xine_event_t *xineEvent)
{
    if (!p || !xineEvent) {
        return;
    }
    //debug() << Q_FUNC_INFO << "Xine event: " << xineEvent->type << QByteArray((char *)xineEvent->data, xineEvent->data_length);

    XineStream *xs = static_cast<XineStream *>(p);

    switch (xineEvent->type) {
    case XINE_EVENT_UI_SET_TITLE: /* request title display change in ui */
        QCoreApplication::postEvent(xs, new QEVENT(NewMetaData));
        break;
    case XINE_EVENT_UI_PLAYBACK_FINISHED: /* frontend can e.g. move on to next playlist entry */
        QCoreApplication::postEvent(xs, new QEVENT(MediaFinished));
        break;
    case XINE_EVENT_PROGRESS: /* index creation/network connections */
        {
            xine_progress_data_t *progress = static_cast<xine_progress_data_t *>(xineEvent->data);
            QCoreApplication::postEvent(xs, new ProgressEvent(QString::fromUtf8(progress->description), progress->percent));
        }
        break;
    case XINE_EVENT_SPU_BUTTON: // the mouse pointer enter/leave a button, used to change the cursor
        {
            xine_spu_button_t *button = static_cast<xine_spu_button_t *>(xineEvent->data);
            if (button->direction == 1) { // enter a button
                xs->handleDownstreamEvent(new QEVENT(NavButtonIn));
            } else {
                xs->handleDownstreamEvent(new QEVENT(NavButtonOut));
            }
        }
        break;
    case XINE_EVENT_UI_CHANNELS_CHANGED:    /* inform ui that new channel info is available */
        debug() << Q_FUNC_INFO << "XINE_EVENT_UI_CHANNELS_CHANGED";
        {
            QCoreApplication::postEvent(xs, new QEVENT(UiChannelsChanged));
        }
        break;
    case XINE_EVENT_UI_MESSAGE:             /* message (dialog) for the ui to display */
        {
            debug() << Q_FUNC_INFO << "XINE_EVENT_UI_MESSAGE";
            const xine_ui_message_data_t *message = static_cast<xine_ui_message_data_t *>(xineEvent->data);
            if (message->type == XINE_MSG_AUDIO_OUT_UNAVAILABLE) {
                debug() << Q_FUNC_INFO << "XINE_MSG_AUDIO_OUT_UNAVAILABLE";
                // we don't know for sure which AudioOutput failed. but the one without any
                // capabilities must be the guilty one
                xs->handleDownstreamEvent(new QEVENT(AudioDeviceFailed));
            }
        }
        break;
    case XINE_EVENT_FRAME_FORMAT_CHANGE:    /* e.g. aspect ratio change during dvd playback */
        debug() << Q_FUNC_INFO << "XINE_EVENT_FRAME_FORMAT_CHANGE";
        {
            xine_format_change_data_t *data = static_cast<xine_format_change_data_t *>(xineEvent->data);
            xs->handleDownstreamEvent(new FrameFormatChangeEvent(data->width, data->height, data->aspect, data->pan_scan));
        }
        break;
    case XINE_EVENT_AUDIO_LEVEL:            /* report current audio level (l/r/mute) */
        debug() << Q_FUNC_INFO << "XINE_EVENT_AUDIO_LEVEL";
        break;
    case XINE_EVENT_QUIT:                   /* last event sent when stream is disposed */
        debug() << Q_FUNC_INFO << "XINE_EVENT_QUIT";
        break;
    case XINE_EVENT_UI_NUM_BUTTONS:         /* number of buttons for interactive menus */
        debug() << Q_FUNC_INFO << "XINE_EVENT_UI_NUM_BUTTONS";
        break;
    case XINE_EVENT_DROPPED_FRAMES:         /* number of dropped frames is too high */
        debug() << Q_FUNC_INFO << "XINE_EVENT_DROPPED_FRAMES";
        break;
    case XINE_EVENT_MRL_REFERENCE_EXT:      /* demuxer->frontend: MRL reference(s) for the real stream */
        {
            xine_mrl_reference_data_ext_t *reference = static_cast<xine_mrl_reference_data_ext_t *>(xineEvent->data);
            debug() << Q_FUNC_INFO << "XINE_EVENT_MRL_REFERENCE_EXT: " << reference->alternative
                << ", " << reference->start_time
                << ", " << reference->duration
                << ", " << reference->mrl
                << ", " << (reference->mrl + strlen(reference->mrl) + 1)
                ;
            QCoreApplication::postEvent(xs, new ReferenceEvent(reference->alternative, reference->mrl));
        }
        break;
    }
}

// called from main thread
XineStream::XineStream()
    : QObject(0), // XineStream is ref-counted
    SourceNodeXT("MediaObject"),
    m_stream(0),
    m_event_queue(0),
    m_xine(Backend::xineEngineForStream()),
    m_nullAudioPort(0),
    m_nullVideoPort(0),
    m_state(Phonon::LoadingState),
    m_prefinishMarkTimer(0),
    m_errorType(Phonon::NoError),
    m_lastSeekCommand(0),
    m_volume(100),
//    m_startTime(-1),
    m_totalTime(-1),
    m_currentTime(-1),
    m_availableSubtitles(-1),
    m_availableAudioChannels(-1),
    m_availableTitles(-1),
    m_availableChapters(-1),
    m_availableAngles(-1),
    m_currentAngle(-1),
    m_currentTitle(-1),
    m_currentChapter(-1),
    m_transitionGap(0),
    m_streamInfoReady(false),
    m_hasVideo(false),
    m_isSeekable(false),
    m_useGaplessPlayback(false),
    m_prefinishMarkReachedNotEmitted(true),
    m_ticking(false),
    m_closing(false),
    m_tickTimer(this)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    connect(&m_tickTimer, SIGNAL(timeout()), SLOT(emitTick()), Qt::DirectConnection);
}

XineStream::~XineStream()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if(m_event_queue) {
        xine_event_dispose_queue(m_event_queue);
        m_event_queue = 0;
    }
    if(m_stream) {
        // FIXME: when shutting down xine_dispose causes a crash 50% of the time. I failed to find
        // the cause after many hours of searching
        if (!Backend::inShutdown()) {
            xine_dispose(m_stream);
        }
        m_stream = 0;
    }
    delete m_prefinishMarkTimer;
    m_prefinishMarkTimer = 0;
    if (m_nullAudioPort) {
        xine_close_audio_driver(m_xine, m_nullAudioPort);
        m_nullAudioPort = 0;
    }
    if (m_nullVideoPort) {
        xine_close_video_driver(m_xine, m_nullVideoPort);
        m_nullVideoPort = 0;
    }
    Backend::returnXineEngine(m_xine);
}

xine_audio_port_t *XineStream::nullAudioPort() const
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!m_nullAudioPort) {
        m_nullAudioPort = xine_open_audio_driver(m_xine, "none", 0);
        Q_ASSERT(m_nullAudioPort);
        if (!m_nullAudioPort) {
            //error(Phonon::FatalError, i18n("Your xine installation is incomplete. Please install the \"none\" output plugin for xine."));
        }
    }
    return m_nullAudioPort;
}

xine_video_port_t *XineStream::nullVideoPort() const
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!m_nullVideoPort) {
        m_nullVideoPort = xine_open_video_driver(m_xine, "auto", XINE_VISUAL_TYPE_NONE, 0);
        Q_ASSERT(m_nullVideoPort);
    }
    return m_nullVideoPort;
}

// any thread
XineEngine XineStream::xine() const
{
    Q_ASSERT(m_xine);
    return m_xine;
}

// xine thread
bool XineStream::xineOpen(Phonon::State newstate)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    Q_ASSERT(m_stream);
    if (m_mrl.isEmpty() || m_closing) {
        return false;
    }
    // only call xine_open if it's not already open
    Q_ASSERT(xine_get_status(m_stream) == XINE_STATUS_IDLE);

#ifdef DISABLE_FILE_MRLS
    if (m_mrl.startsWith("file:/")) {
        debug() << Q_FUNC_INFO << "faked xine_open failed for m_mrl =" << m_mrl.constData();
        error(Phonon::NormalError, tr("Cannot open media data at '<i>%1</i>'").arg(m_mrl.constData()));
        return false;
    }
#endif

    // xine_open can call functions from ByteStream which will block waiting for data.
    //debug() << Q_FUNC_INFO << "xine_open(" << m_mrl.constData() << ")";
    if (xine_open(m_stream, m_mrl.constData()) == 0) {
        debug() << Q_FUNC_INFO << "xine_open failed for m_mrl =" << m_mrl.constData();
        switch (xine_get_error(m_stream)) {
        case XINE_ERROR_NONE:
            // hmm?
            abort();
        case XINE_ERROR_NO_INPUT_PLUGIN:
            error(Phonon::NormalError, tr("Cannot find input plugin for MRL [%1]").arg(m_mrl.constData()));
            break;
        case XINE_ERROR_NO_DEMUX_PLUGIN:
            if (m_mrl.startsWith("kbytestream:/")) {
                error(Phonon::FatalError, tr("Cannot find demultiplexer plugin for the given media data"));
            } else {
                error(Phonon::FatalError, tr("Cannot find demultiplexer plugin for MRL [%1]").arg(m_mrl.constData()));
            }
            break;
        default:
            {
                const char *const *logs = xine_get_log(m_xine, XINE_LOG_MSG);
                error(Phonon::NormalError, QString::fromUtf8(logs[0]));
            }
            break;
//X         default:
//X             error(Phonon::NormalError, i18n("Cannot open media data at '<i>%1</i>'", m_mrl.constData()));
//X             break;
        }
        return false;
    }
    debug() << Q_FUNC_INFO << "xine_open succeeded for m_mrl =" << m_mrl.constData();

    m_lastTimeUpdate.tv_sec = 0;
    xine_get_pos_length(m_stream, 0, &m_currentTime, &m_totalTime);
    getStreamInfo();
    emit length(m_totalTime);
    updateMetaData();
    // if there's a PlayCommand in the event queue the state should not go to StoppedState
    changeState(newstate);
    return true;
}

// called from main thread
int XineStream::totalTime() const
{
    if (!m_stream || m_mrl.isEmpty()) {
        return -1;
    }
    return m_totalTime;
}

// called from main thread
int XineStream::remainingTime() const
{
    if (!m_stream || m_mrl.isEmpty()) {
        return 0;
    }
    QMutexLocker locker(&m_updateTimeMutex);
    if (m_state == Phonon::PlayingState && m_lastTimeUpdate.tv_sec > 0) {
        struct timeval now;
        gettimeofday(&now, 0);
        const int diff = (now.tv_sec - m_lastTimeUpdate.tv_sec) * 1000 + (now.tv_usec - m_lastTimeUpdate.tv_usec) / 1000;
        return m_totalTime - (m_currentTime + diff);
    }
    return m_totalTime - m_currentTime;
}

// called from main thread
int XineStream::currentTime() const
{
    if (!m_stream || m_mrl.isEmpty()) {
        return -1;
    }
    QMutexLocker locker(&m_updateTimeMutex);
    if (m_state == Phonon::PlayingState && m_lastTimeUpdate.tv_sec > 0) {
        struct timeval now;
        gettimeofday(&now, 0);
        const int diff = (now.tv_sec - m_lastTimeUpdate.tv_sec) * 1000 + (now.tv_usec - m_lastTimeUpdate.tv_usec) / 1000;
        return m_currentTime + diff;
    }
    return m_currentTime;
}

// called from main thread
bool XineStream::hasVideo() const
{
    if (!m_streamInfoReady) {
        QMutexLocker locker(&m_streamInfoMutex);
        QCoreApplication::postEvent(const_cast<XineStream *>(this), new QEVENT(GetStreamInfo));
        // wait a few ms, perhaps the other thread finishes the event in time and this method
        // can return a useful value
        // FIXME: this is non-deterministic: a program might fail sometimes and sometimes work
        // because of this
        if (!m_waitingForStreamInfo.wait(&m_streamInfoMutex, 80)) {
            debug() << Q_FUNC_INFO << "waitcondition timed out";
        }
    }
    return m_hasVideo;
}

// called from main thread
bool XineStream::isSeekable() const
{
    if (!m_streamInfoReady) {
        //QMutexLocker locker(&m_streamInfoMutex);
        QCoreApplication::postEvent(const_cast<XineStream *>(this), new QEVENT(GetStreamInfo));
        // wait a few ms, perhaps the other thread finishes the event in time and this method
        // can return a useful value
        // FIXME: this is non-deterministic: a program might fail sometimes and sometimes work
        // because of this
        /*if (!m_waitingForStreamInfo.wait(&m_streamInfoMutex, 80)) {
            debug() << Q_FUNC_INFO << "waitcondition timed out";
            return false;
        } */
    }
    return m_isSeekable;
}

// xine thread
void XineStream::getStreamInfo()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());

    if (m_stream && !m_mrl.isEmpty()) {
        if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
            debug() << Q_FUNC_INFO << "calling xineOpen from ";
            if (!xineOpen(Phonon::StoppedState)) {
                return;
            }
        }
        QMutexLocker locker(&m_streamInfoMutex);
        bool hasVideo   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_HAS_VIDEO);
        bool isSeekable = xine_get_stream_info(m_stream, XINE_STREAM_INFO_SEEKABLE);
        int availableTitles   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_TITLE_COUNT);
        int availableChapters = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT);
        int availableAngles   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_ANGLE_COUNT);
        int availableSubtitles = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
        int availableAudioChannels = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL);
        m_streamInfoReady = true;
        if (m_hasVideo != hasVideo) {
            m_hasVideo = hasVideo;
            emit hasVideoChanged(m_hasVideo);
        }
        if (m_isSeekable != isSeekable) {
            m_isSeekable = isSeekable;
            emit seekableChanged(m_isSeekable);
        }
        if (m_availableTitles != availableTitles) {
            debug() << Q_FUNC_INFO << "available titles changed: " << availableTitles;
            m_availableTitles = availableTitles;
            emit availableTitlesChanged(m_availableTitles);
        }
        if (m_availableChapters != availableChapters) {
            debug() << Q_FUNC_INFO << "available chapters changed: " << availableChapters;
            m_availableChapters = availableChapters;
            emit availableChaptersChanged(m_availableChapters);
        }
        if (m_availableAngles != availableAngles) {
            debug() << Q_FUNC_INFO << "available angles changed: " << availableAngles;
            m_availableAngles = availableAngles;
            emit availableAnglesChanged(m_availableAngles);
        }
        if (m_availableSubtitles != availableSubtitles) {
            debug() << Q_FUNC_INFO << "available angles changed: " << availableSubtitles;
            m_availableSubtitles = availableSubtitles;
            emit availableSubtitlesChanged();
        }
        if (m_availableAudioChannels != availableAudioChannels) {
            debug() << Q_FUNC_INFO << "available angles changed: " << availableAudioChannels;
            m_availableAudioChannels = availableAudioChannels;
            emit availableAudioChannelsChanged();
        }
        if (m_hasVideo) {
            uint32_t width = xine_get_stream_info(m_stream, XINE_STREAM_INFO_VIDEO_WIDTH);
            uint32_t height = xine_get_stream_info(m_stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
            handleDownstreamEvent(new FrameFormatChangeEvent(width, height, 0, 0));
        }
    }
    m_waitingForStreamInfo.wakeAll();
}

// xine thread
bool XineStream::createStream()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());

    if (m_stream || m_state == Phonon::ErrorState) {
        return false;
    }

    m_portMutex.lock();
    //debug() << Q_FUNC_INFO << "AudioPort.xinePort() = " << m_audioPort.xinePort();
    xine_audio_port_t *audioPort = 0;
    xine_video_port_t *videoPort = 0;
    Q_ASSERT(m_mediaObject);
    QSet<SinkNode *> sinks = m_mediaObject->sinks();
    debug() << Q_FUNC_INFO << "MediaObject is connected to " << sinks.size() << " nodes";
    foreach (SinkNode *sink, sinks) {
        Q_ASSERT(sink->threadSafeObject());
        if (sink->threadSafeObject()->audioPort()) {
            Q_ASSERT(audioPort == 0);
            audioPort = sink->threadSafeObject()->audioPort();
        }
        if (sink->threadSafeObject()->videoPort()) {
            Q_ASSERT(videoPort == 0);
            videoPort = sink->threadSafeObject()->videoPort();
        }
    }
    if (!audioPort) {
        debug() << Q_FUNC_INFO << "creating xine_stream with null audio port";
        audioPort = nullAudioPort();
    }
    if (!videoPort) {
        debug() << Q_FUNC_INFO << "creating xine_stream with null video port";
        videoPort = nullVideoPort();
    }
    m_stream = xine_stream_new(m_xine, audioPort, videoPort);
    hackSetProperty("xine_stream_t", QVariant::fromValue(static_cast<void *>(m_stream)));

    if (m_volume != 100) {
        xine_set_param(m_stream, XINE_PARAM_AUDIO_AMP_LEVEL, m_volume);
    }
//X     if (!m_audioPort.isValid()) {
//X         xine_set_param(m_stream, XINE_PARAM_IGNORE_AUDIO, 1);
//X     }
//X     if (!m_videoPort) {
//X         xine_set_param(m_stream, XINE_PARAM_IGNORE_VIDEO, 1);
//X     }
    m_portMutex.unlock();
    m_waitingForRewire.wakeAll();

    Q_ASSERT(!m_event_queue);
    m_event_queue = xine_event_new_queue(m_stream);
    xine_event_create_listener_thread(m_event_queue, &XineStream::xineEventListener, (void *)this);

    if (m_useGaplessPlayback) {
        debug() << Q_FUNC_INFO << "XINE_PARAM_EARLY_FINISHED_EVENT: 1";
        xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);
#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
    } else if (m_transitionGap > 0) {
        debug() << Q_FUNC_INFO << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
        xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
#endif // XINE_PARAM_DELAY_FINISHED_EVENT
    } else {
        debug() << Q_FUNC_INFO << "XINE_PARAM_EARLY_FINISHED_EVENT: 0";
        xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 0);
    }

    return true;
}

/*
//called from main thread
void XineStream::addAudioPostList(const AudioPostList &postList)
{
    QCoreApplication::postEvent(this, new ChangeAudioPostListEvent(postList, ChangeAudioPostListEvent::Add));
}

//called from main thread
void XineStream::removeAudioPostList(const AudioPostList &postList)
{
    QCoreApplication::postEvent(this, new ChangeAudioPostListEvent(postList, ChangeAudioPostListEvent::Remove));
}
*/

//called from main thread
void XineStream::aboutToDeleteVideoWidget()
{
    debug() << Q_FUNC_INFO;
    m_portMutex.lock();

    // schedule m_stream rewiring
    QCoreApplication::postEvent(this, new QEVENT(RewireVideoToNull));
    debug() << Q_FUNC_INFO << "waiting for rewire";
    m_waitingForRewire.wait(&m_portMutex);
    m_portMutex.unlock();
}

// called from main thread
void XineStream::setTickInterval(qint32 interval)
{
    QCoreApplication::postEvent(this, new SetTickIntervalEvent(interval));
}

// called from main thread
void XineStream::setPrefinishMark(qint32 time)
{
    QCoreApplication::postEvent(this, new SetPrefinishMarkEvent(time));
}

// called from main thread
void XineStream::useGaplessPlayback(bool b)
{
    if (m_useGaplessPlayback == b) {
        return;
    }
    m_useGaplessPlayback = b;
    QCoreApplication::postEvent(this, new QEVENT(TransitionTypeChanged));
}

// called from main thread
void XineStream::useGapOf(int gap)
{
    m_useGaplessPlayback = false;
    m_transitionGap = gap;
    QCoreApplication::postEvent(this, new QEVENT(TransitionTypeChanged));
}

// called from main thread
void XineStream::gaplessSwitchTo(const QByteArray &mrl)
{
    QCoreApplication::postEvent(this, new GaplessSwitchEvent(mrl));
}

// xine thread
void XineStream::changeState(Phonon::State newstate)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (m_state == newstate) {
        return;
    }
    Phonon::State oldstate = m_state;
    m_state = newstate;
    if (newstate == Phonon::PlayingState) {
        if (m_ticking) {
            m_tickTimer.start();
            //debug() << Q_FUNC_INFO << "tickTimer started.";
        }
        if (m_prefinishMark > 0) {
            emitAboutToFinish();
        }
    } else if (oldstate == Phonon::PlayingState) {
        m_tickTimer.stop();
        //debug() << Q_FUNC_INFO << "tickTimer stopped.";
        m_prefinishMarkReachedNotEmitted = true;
        if (m_prefinishMarkTimer) {
            m_prefinishMarkTimer->stop();
        }
    }
    if (newstate == Phonon::ErrorState) {
        debug() << Q_FUNC_INFO << "reached error state";// from: " << kBacktrace();
        if (m_event_queue) {
            xine_event_dispose_queue(m_event_queue);
            m_event_queue = 0;
        }
        if (m_stream) {
            xine_dispose(m_stream);
            m_stream = 0;
            hackSetProperty("xine_stream_t", QVariant());
        }
    }
    emit stateChanged(newstate, oldstate);
}

static bool checkIsUtf8(const char *buf)
{
    if (!buf || !*buf) {
        // it's actually neither true nor false, but the logic below needs us to return false here
        // for empty/null strings
        return false;
    }

    bool isValidUtf8 = false;
    unsigned char c;
    for (int i = 0; (c = buf[i]); ++i) {
        if ((c & 0x80) == 0) {
            // 0xxxxxxx is ASCII
            continue;
        }
        if ((c & 0x40) == 0) {
            // 10xxxxxx is never UTF-8
            return false;
        } else {
            // 11xxxxxx begins UTF-8
            int following;

            if ((c & 0x20) == 0) {
                // 110xxxxx
                if (c == 0xC0 || c == 0xC1) {
                    return false;
                }
                following = 1;
            } else if ((c & 0x10) == 0) {
                // 1110xxxx
                following = 2;
            } else if ((c & 0x08) == 0) {
                // 11110xxx
                if (c == 0xF5) {
                    return false;
                }
                following = 3;
            } else if ((c & 0x04) == 0) {
                // 111110xx
                following = 4;
            } else if ((c & 0x02) == 0) {
                // 1111110x
                following = 5;
            } else {
                return false;
            }

            for (int n = 0; n < following; ++n) {
                ++i;
                if (!(c = buf[i])) {
                    // null termination, this should make it invalid UTF-8
                    return false;
                }

                if ((c & 0xC0) != 0x80) {
                    return false;
                }
            }
            isValidUtf8 = true;
        }
    }
    return isValidUtf8;
}

// xine thread
void XineStream::updateMetaData()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    const char *meta[8] = {
        xine_get_meta_info(m_stream, XINE_META_INFO_TITLE),
        xine_get_meta_info(m_stream, XINE_META_INFO_ARTIST),
        xine_get_meta_info(m_stream, XINE_META_INFO_GENRE),
        xine_get_meta_info(m_stream, XINE_META_INFO_ALBUM),
        xine_get_meta_info(m_stream, XINE_META_INFO_YEAR),
        xine_get_meta_info(m_stream, XINE_META_INFO_TRACK_NUMBER),
        xine_get_meta_info(m_stream, XINE_META_INFO_COMMENT),
        xine_get_meta_info(m_stream, XINE_META_INFO_CDINDEX_DISCID)
    };
    bool isUtf8 = false;
    for (int i = 0; !isUtf8 && i < 8; ++i) {
        isUtf8 &= checkIsUtf8(meta[i]);
    }
    QTextCodec *codec = QTextCodec::codecForMib(106); // utf-8
    if (!isUtf8) {
        QTextCodec *localCodec = QTextCodec::codecForLocale();
        if (localCodec == codec) {
            // if the local codec also is UTF-8 our best guess is Latin-1
            codec = QTextCodec::codecForName("ISO 8859-1");
        } else {
            // hoping that the local codec is the same as the one used in the meta data
            codec = localCodec;
        }
    }
    QMultiMap<QString, QString> metaDataMap;
    metaDataMap.insert(QLatin1String("TITLE"), codec->toUnicode(meta[0]));
    metaDataMap.insert(QLatin1String("ARTIST"), codec->toUnicode(meta[1]));
    metaDataMap.insert(QLatin1String("GENRE"), codec->toUnicode(meta[2]));
    metaDataMap.insert(QLatin1String("ALBUM"), codec->toUnicode(meta[3]));
    metaDataMap.insert(QLatin1String("DATE"), codec->toUnicode(meta[4]));
    metaDataMap.insert(QLatin1String("TRACKNUMBER"), codec->toUnicode(meta[5]));
    metaDataMap.insert(QLatin1String("DESCRIPTION"), codec->toUnicode(meta[6]));
    metaDataMap.insert(QLatin1String("MUSICBRAINZ_DISCID"), codec->toUnicode(meta[7]));
    if(metaDataMap == m_metaDataMap)
        return;
    m_metaDataMap = metaDataMap;
    //debug() << Q_FUNC_INFO << "emitting metaDataChanged(" << m_metaDataMap << ")";
    emit metaDataChanged(m_metaDataMap);
}

// xine thread
void XineStream::playbackFinished()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    {
        QMutexLocker locker(&m_mutex);
        if (m_prefinishMarkReachedNotEmitted && m_prefinishMark > 0) {
            emit prefinishMarkReached(0);
        }
        changeState(Phonon::StoppedState);
        xine_close(m_stream); // TODO: is it necessary? should xine_close be called as late as possible?
        m_streamInfoReady = false;
        m_prefinishMarkReachedNotEmitted = true;
        emit finished();
    }
    m_waitingForClose.wakeAll();
}

// xine thread
inline void XineStream::error(Phonon::ErrorType type, const QString &string)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    debug() << Q_FUNC_INFO << type << string;
    m_errorLock.lockForWrite();
    m_errorType = type;
    m_errorString = string;
    m_errorLock.unlock();
    changeState(Phonon::ErrorState);
}

const char *nameForEvent(int e)
{
    switch (e) {
    case Event::Reference:
        return "Reference";
    case Event::UiChannelsChanged:
        return "UiChannelsChanged";
    case Event::MediaFinished:
        return "MediaFinished";
    case Event::UpdateTime:
        return "UpdateTime";
    case Event::GaplessSwitch:
        return "GaplessSwitch";
    case Event::NewMetaData:
        return "NewMetaData";
    case Event::Progress:
        return "Progress";
    case Event::GetStreamInfo:
        return "GetStreamInfo";
    case Event::UpdateVolume:
        return "UpdateVolume";
    case Event::MrlChanged:
        return "MrlChanged";
    case Event::TransitionTypeChanged:
        return "TransitionTypeChanged";
    case Event::RewireVideoToNull:
        return "RewireVideoToNull";
    case Event::PlayCommand:
        return "PlayCommand";
    case Event::PauseCommand:
        return "PauseCommand";
    case Event::StopCommand:
        return "StopCommand";
    case Event::UnloadCommand:
        return "UnloadCommand";
    case Event::SetTickInterval:
        return "SetTickInterval";
    case Event::SetPrefinishMark:
        return "SetPrefinishMark";
    case Event::SeekCommand:
        return "SeekCommand";
    //case Event::EventSend:
        //return "EventSend";
    case Event::SetParam:
        return "SetParam";
        /*
    case Event::ChangeAudioPostList:
        return "ChangeAudioPostList";
        */
    default:
        return 0;
    }
}

// xine thread
bool XineStream::event(QEvent *ev)
{
    if (ev->type() != QEvent::ThreadChange) {
        Q_ASSERT(QThread::currentThread() == XineThread::instance());
    }
    const char *eventName = nameForEvent(ev->type());
    if (m_closing) {
        // when closing all events except MrlChanged are ignored. MrlChanged is used to detach from
        // a kbytestream:/ MRL
        switch (ev->type()) {
        case Event::MrlChanged:
        //case Event::ChangeAudioPostList:
            break;
        default:
            if (eventName) {
                debug() << Q_FUNC_INFO << "####################### ignoring Event: " << eventName;
            }
            return QObject::event(ev);
        }
    }
    if (eventName) {
        if (static_cast<int>(ev->type()) == Event::Progress) {
            ProgressEvent *e = static_cast<ProgressEvent *>(ev);
            debug() << Q_FUNC_INFO << "################################ Event: " << eventName << ": " << e->percent;
        } else {
            debug() << Q_FUNC_INFO << "################################ Event: " << eventName;
        }
    }
    switch (ev->type()) {
    case Event::Reference:
        ev->accept();
        {
            ReferenceEvent *e = static_cast<ReferenceEvent *>(ev);
            setMrlInternal(e->mrl);
            if (xine_get_status(m_stream) != XINE_STATUS_IDLE) {
                m_mutex.lock();
                xine_close(m_stream);
                m_streamInfoReady = false;
                m_prefinishMarkReachedNotEmitted = true;
                m_mutex.unlock();
            }
            if (xineOpen(Phonon::BufferingState)) {
                internalPlay();
            }
        }
        return true;
    case Event::UiChannelsChanged:
        ev->accept();
        // check chapter, title, angle and substreams
        if (m_stream) {
            int availableTitles   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_TITLE_COUNT);
            int availableChapters = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT);
            int availableAngles   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_ANGLE_COUNT);
            if (m_availableTitles != availableTitles) {
                debug() << Q_FUNC_INFO << "available titles changed: " << availableTitles;
                m_availableTitles = availableTitles;
                emit availableTitlesChanged(m_availableTitles);
            }
            if (m_availableChapters != availableChapters) {
                debug() << Q_FUNC_INFO << "available chapters changed: " << availableChapters;
                m_availableChapters = availableChapters;
                emit availableChaptersChanged(m_availableChapters);
            }
            if (m_availableAngles != availableAngles) {
                debug() << Q_FUNC_INFO << "available angles changed: " << availableAngles;
                m_availableAngles = availableAngles;
                emit availableAnglesChanged(m_availableAngles);
            }

            {
                const int availableSubtitles = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
                if(availableSubtitles != m_availableSubtitles)
                {
                    debug() << Q_FUNC_INFO << "available subtitles changed: " << availableSubtitles;
                    m_availableSubtitles = availableSubtitles;
                    emit availableSubtitlesChanged();
                }
            }
            {
                const int availableAudioChannels = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL);
                if(availableAudioChannels != m_availableAudioChannels)
                {
                    debug() << Q_FUNC_INFO << "available audio channels changed: " << availableAudioChannels;
                    m_availableAudioChannels = availableAudioChannels;
                    emit availableAudioChannelsChanged();
                }
            }

            int currentTitle   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER);
            int currentChapter = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER);
            int currentAngle   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER);
            if (currentAngle != m_currentAngle) {
                debug() << Q_FUNC_INFO << "current angle changed: " << currentAngle;
                m_currentAngle = currentAngle;
                emit angleChanged(m_currentAngle);
            }
            if (currentChapter != m_currentChapter) {
                debug() << Q_FUNC_INFO << "current chapter changed: " << currentChapter;
                m_currentChapter = currentChapter;
                emit chapterChanged(m_currentChapter);
            }
            if (currentTitle != m_currentTitle) {
                debug() << Q_FUNC_INFO << "current title changed: " << currentTitle;
                m_currentTitle = currentTitle;
                emit titleChanged(m_currentTitle);
            }
        }
        return true;
    case Event::Error:
        ev->accept();
        {
            ErrorEvent *e = static_cast<ErrorEvent *>(ev);
            error(e->type, e->reason);
        }
        return true;
    case Event::PauseForBuffering:
        ev->accept();
        if (m_stream) {
            xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE); //_x_set_speed (m_stream, XINE_SPEED_PAUSE);
            streamClock(m_stream)->set_option (streamClock(m_stream), CLOCK_SCR_ADJUSTABLE, 0);
        }
        return true;
    case Event::UnpauseForBuffering:
        ev->accept();
        if (m_stream) {
           if (Phonon::PausedState != m_state) {
               xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL); //_x_set_speed (m_stream, XINE_SPEED_NORMAL);
           }
           streamClock(m_stream)->set_option (streamClock(m_stream), CLOCK_SCR_ADJUSTABLE, 1);
        }
        return true;
    case Event::EventSend:
        ev->accept();
        {
            EventSendEvent *e = static_cast<EventSendEvent *>(ev);
            if (m_stream) {
                xine_event_send(m_stream, e->event);
            }
            switch (e->event->type) {
            case XINE_EVENT_INPUT_MOUSE_MOVE:
            case XINE_EVENT_INPUT_MOUSE_BUTTON:
                delete static_cast<xine_input_data_t *>(e->event->data);
                break;
            }
            delete e->event;
        }
        return true;
    case Event::SetParam:
        ev->accept();
        if (m_stream) {
            SetParamEvent *e = static_cast<SetParamEvent *>(ev);
            xine_set_param(m_stream, e->param, e->value);
        }
        return true;
    case Event::MediaFinished:
        ev->accept();
        debug() << Q_FUNC_INFO << "MediaFinishedEvent m_useGaplessPlayback = " << m_useGaplessPlayback;
        if (m_stream) {
            if (m_useGaplessPlayback) {
                xine_set_param(m_stream, XINE_PARAM_GAPLESS_SWITCH, 1);
            }
            emit needNextUrl();
        }
        return true;
    case Event::UpdateTime:
        updateTime();
        ev->accept();
        return true;
    case Event::GaplessSwitch:
        ev->accept();
        {
            GaplessSwitchEvent *e = static_cast<GaplessSwitchEvent *>(ev);
            m_mutex.lock();
            if (e->mrl.isEmpty()) {
                debug() << Q_FUNC_INFO << "no GaplessSwitch";
            } else {
                setMrlInternal(e->mrl);
                debug() << Q_FUNC_INFO << "GaplessSwitch new m_mrl =" << m_mrl.constData();
            }
            if (e->mrl.isEmpty() || m_closing) {
                xine_set_param(m_stream, XINE_PARAM_GAPLESS_SWITCH, 0);
                m_mutex.unlock();
                playbackFinished();
                return true;
            }
            if (!xine_open(m_stream, m_mrl.constData())) {
                qWarning("xine_open for gapless playback failed!");
                xine_set_param(m_stream, XINE_PARAM_GAPLESS_SWITCH, 0);
                m_mutex.unlock();
                playbackFinished();
                return true; // FIXME: correct?
            }
            m_mutex.unlock();
            xine_play(m_stream, 0, 0);

            if (m_prefinishMarkReachedNotEmitted && m_prefinishMark > 0) {
                emit prefinishMarkReached(0);
            }
            m_prefinishMarkReachedNotEmitted = true;
            getStreamInfo();
            updateTime();
            updateMetaData();
        }
        return true;
    case Event::NewMetaData:
        ev->accept();
        if (m_stream) {
            getStreamInfo();
            updateMetaData();
        }
        return true;
    case Event::Progress:
        {
            ProgressEvent *e = static_cast<ProgressEvent *>(ev);
            if (e->percent < 100) {
                if (m_state == Phonon::PlayingState) {
                    changeState(Phonon::BufferingState);
                }
            } else {
                if (m_state == Phonon::BufferingState) {
                    changeState(Phonon::PlayingState);
                }
                //QTimer::singleShot(20, this, SLOT(getStartTime()));
            }
            debug() << Q_FUNC_INFO << "emit bufferStatus(" << e->percent << ")";
            emit bufferStatus(e->percent);
        }
        ev->accept();
        return true;
    case Event::GetStreamInfo:
        ev->accept();
        if (m_stream) {
            getStreamInfo();
        }
        return true;
    case Event::UpdateVolume:
        ev->accept();
        {
            UpdateVolumeEvent *e = static_cast<UpdateVolumeEvent *>(ev);
            m_volume = e->volume;
            if (m_stream) {
                xine_set_param(m_stream, XINE_PARAM_AUDIO_AMP_LEVEL, m_volume);
            }
        }
        return true;
    case Event::RequestSnapshot:
        ev->accept();
        if (m_stream) {
            const int32_t w = xine_get_stream_info(m_stream, XINE_STREAM_INFO_VIDEO_WIDTH);
            const int32_t h = xine_get_stream_info(m_stream, XINE_STREAM_INFO_VIDEO_HEIGHT);
            debug() << Q_FUNC_INFO << "taking snapshot of" << w << h;
            if (w > 0 && h > 0) {
                int width, height, ratio_code, format;
                QVarLengthArray<uint8_t> img(w * h * 4);
                int success = xine_get_current_frame (m_stream, &width, &height, &ratio_code,
                        &format, &img[0]);
                if (!success) {
                    return true;
                }
                Q_ASSERT(w * h * 2 >= width * height);
                QImage qimg(width, height, QImage::Format_RGB32);
                /*
                 * Do YCbCr - sRGB conversion according to ITU-R BT.709 with Kb = 0.0722 and Kr = 0.2126
                 *
                 *     /   \     /                                        \     /        \
                 *    |  R  |   |  76309.0411,      0.0,      117489.0789  |   |  Y - 16  |
                 *    |     |   |                                          |   |          |
                 * => |  G  | = |  76309.0411, -13975.46119, -34924.74576  | * | Cb - 128 | * 2^-16
                 *    |     |   |                                          |   |          |
                 *    |  B  |   |  76309.0411,  138438.3634,       0       |   | Cr - 128 |
                 *     \   /     \                                        /     \        /
                 */
                switch (format) {
                case XINE_IMGFMT_YUY2: // every four consecutive pixels Y0 Cb Y1 Cr
                    debug() << Q_FUNC_INFO << "got a YUY2 snapshot";
                    Q_ASSERT(width % 2 == 0);
                    for (int row = 0; row < height; ++row) {
                        QRgb *line = reinterpret_cast<QRgb *>(qimg.scanLine(row));
                        const uint8_t *yuyv = &img[2 * width];
                        for (int col = 0; col < 2 * width; col += 4) {
                            const int y0 = (yuyv[col] - 16) * 76309;
                            const int u  = yuyv[col + 1] - 128;
                            const int y1 = (yuyv[col + 2] - 16) * 76309;
                            const int v  = yuyv[col + 3] - 128;
                            const int r  =            117489 * v + 16384;
                            const int g  = -13975 * u -34925 * v + 16384;
                            const int b  = 138438 * u            + 16384;
                            line[col >> 1] = qRgb(
                                    qBound(0, (y0 + r) >> 16, 255),
                                    qBound(0, (y0 + g) >> 16, 255),
                                    qBound(0, (y0 + b) >> 16, 255));
                            line[(col >> 1) + 1] = qRgb(
                                    qBound(0, (y1 + r) >> 16, 255),
                                    qBound(0, (y1 + g) >> 16, 255),
                                    qBound(0, (y1 + b) >> 16, 255));
                        }
                    }
                    break;
                case XINE_IMGFMT_YV12:
                    debug() << Q_FUNC_INFO << "got a YV12 snapshot";
                    Q_ASSERT(width % 2 == 0);
                    Q_ASSERT(height % 2 == 0);
                    {
                        const int w2 = width >> 1;
                        const uint8_t *yplane = &img[0];
                        const uint8_t *uplane = &img[width * height];
                        const uint8_t *vplane = &img[width * height + ((width * height) >> 2)];
                        for (int row = 0; row < height; row += 2) {
                            QRgb *line0 = reinterpret_cast<QRgb *>(qimg.scanLine(row));
                            QRgb *line1 = reinterpret_cast<QRgb *>(qimg.scanLine(row + 1));
                            const uint8_t *yline0 = &yplane[row * width];
                            const uint8_t *yline1 = &yplane[(row + 1) * width];
                            const uint8_t *uline = &uplane[(row >> 1) * w2];
                            const uint8_t *vline = &vplane[(row >> 1) * w2];
                            for (int col = 0; col < width; col += 2) {
                                const int y0 = (yline0[col    ] - 16) * 76309;
                                const int y1 = (yline0[col + 1] - 16) * 76309;
                                const int y2 = (yline1[col    ] - 16) * 76309;
                                const int y3 = (yline1[col + 1] - 16) * 76309;
                                const int u  = (uline[col >> 1] - 128);
                                const int v  = (vline[col >> 1] - 128);
                                const int r  =            117489 * v + 16384;
                                const int g  = -13975 * u -34925 * v + 16384;
                                const int b  = 138438 * u            + 16384;
                                line0[col] = qRgb(
                                        qBound(0, (y0 + r) >> 16, 255),
                                        qBound(0, (y0 + g) >> 16, 255),
                                        qBound(0, (y0 + b) >> 16, 255));
                                line0[col + 1] = qRgb(
                                        qBound(0, (y1 + r) >> 16, 255),
                                        qBound(0, (y1 + g) >> 16, 255),
                                        qBound(0, (y1 + b) >> 16, 255));
                                line1[col] = qRgb(
                                        qBound(0, (y2 + r) >> 16, 255),
                                        qBound(0, (y2 + g) >> 16, 255),
                                        qBound(0, (y2 + b) >> 16, 255));
                                line1[col + 1] = qRgb(
                                        qBound(0, (y3 + r) >> 16, 255),
                                        qBound(0, (y3 + g) >> 16, 255),
                                        qBound(0, (y3 + b) >> 16, 255));
                            }
                        }
                    }
                    break;
                default:
                    return true;
                }
                   RequestSnapshotEvent *event=static_cast<RequestSnapshotEvent*>(ev);
                   event->image=qimg;
                   event->waitCondition->wakeAll();
//                 handleDownstreamEvent(new SnapshotReadyEvent(qimg));
            }
        }
        return true;
    case Event::MrlChanged:
        ev->accept();
        {
            MrlChangedEvent *e = static_cast<MrlChangedEvent *>(ev);
            /* Always handle a MRL change request. We assume the application knows what it's
             * doing. If we return here then the stream is not reinitialized and the state
             * changes are different.
            if (m_mrl == e->mrl) {
                return true;
            } */
            State previousState = m_state;
            setMrlInternal(e->mrl);
            m_errorType = Phonon::NoError;
            m_errorString = QString();
            if (!m_stream) {
                changeState(Phonon::LoadingState);
                m_mutex.lock();
                createStream();
                m_mutex.unlock();
                if (!m_stream) {
                    qWarning() << "MrlChangedEvent: createStream didn't create a stream. This should not happen.";
                    error(Phonon::FatalError, tr("Xine failed to create a stream."));
                    return true;
                }
            } else if (xine_get_status(m_stream) != XINE_STATUS_IDLE) {
                m_mutex.lock();
                xine_close(m_stream);
                m_streamInfoReady = false;
                m_prefinishMarkReachedNotEmitted = true;
                changeState(Phonon::LoadingState);
                m_mutex.unlock();
            }
            if (m_closing || m_mrl.isEmpty()) {
                debug() << Q_FUNC_INFO << "MrlChanged: don't call xineOpen. m_closing =" << m_closing << ", m_mrl =" << m_mrl.constData();
                m_waitingForClose.wakeAll();
            } else {
                debug() << Q_FUNC_INFO << "calling xineOpen from MrlChanged";
                if (!xineOpen(Phonon::StoppedState)) {
                    return true;
                }
                switch (e->stateForNewMrl) {
                case StoppedState:
                    break;
                case PlayingState:
                    if (m_stream) {
                        internalPlay();
                    }
                    break;
                case PausedState:
                    if (m_stream) {
                        internalPause();
                    }
                    break;
                case KeepState:
                    switch (previousState) {
                    case Phonon::PlayingState:
                    case Phonon::BufferingState:
                        if (m_stream) {
                            internalPlay();
                        }
                        break;
                    case Phonon::PausedState:
                        if (m_stream) {
                            internalPause();
                        }
                        break;
                    case Phonon::LoadingState:
                    case Phonon::StoppedState:
                    case Phonon::ErrorState:
                        break;
                    }
                }
            }
        }
        return true;
    case Event::TransitionTypeChanged:
        if (m_stream) {
            if (m_useGaplessPlayback) {
                debug() << Q_FUNC_INFO << "XINE_PARAM_EARLY_FINISHED_EVENT: 1";
                xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);
#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
            } else if (m_transitionGap > 0) {
                debug() << Q_FUNC_INFO << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
                xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
#endif // XINE_PARAM_DELAY_FINISHED_EVENT
            } else {
                debug() << Q_FUNC_INFO << "XINE_PARAM_EARLY_FINISHED_EVENT: 0";
                xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 0);
            }
        }
        ev->accept();
        return true;
    case Event::RewireVideoToNull:
        ev->accept();
        {
            QMutexLocker locker(&m_mutex);
            if (!m_stream) {
                return true;
            }
            QMutexLocker portLocker(&m_portMutex);
            debug() << Q_FUNC_INFO << "rewiring ports";
            xine_post_out_t *videoSource = xine_get_video_source(m_stream);
            xine_video_port_t *videoPort = nullVideoPort();
            xine_post_wire_video_port(videoSource, videoPort);
            m_waitingForRewire.wakeAll();
        }
        return true;
    case Event::PlayCommand:
        ev->accept();
        if (m_mediaObject->sinks().isEmpty()) {
            qWarning("request to play a stream, but no valid audio/video outputs are given/available");
            error(Phonon::FatalError, tr("Playback failed because no valid audio or video outputs are available"));
            return true;
        }
        if (m_state == Phonon::ErrorState || m_state == Phonon::PlayingState) {
            return true;
        }
        Q_ASSERT(!m_mrl.isEmpty());
        /*if (m_mrl.isEmpty()) {
            kError(610) << "PlayCommand: m_mrl is empty. This should not happen.";
            error(Phonon::NormalError, i18n("Request to play without media data"));
            return true;
        } */
        if (!m_stream) {
            QMutexLocker locker(&m_mutex);
            createStream();
            if (!m_stream) {
                qWarning() << "PlayCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, tr("Xine failed to create a stream."));
                return true;
            }
        }
        if (m_state == Phonon::PausedState) {
            xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
            changeState(Phonon::PlayingState);
        } else {
            //X                 int total;
            //X                 if (xine_get_pos_length(stream(), 0, &m_startTime, &total) == 1) {
            //X                     if (total > 0 && m_startTime < total && m_startTime >= 0)
            //X                         m_startTime = -1;
            //X                 } else {
            //X                     m_startTime = -1;
            //X                 }
            if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
                debug() << Q_FUNC_INFO << "calling xineOpen from PlayCommand";
                if (!xineOpen(Phonon::BufferingState)) {
                    return true;
                }
            }
            internalPlay();
        }
        return true;
    case Event::PauseCommand:
        ev->accept();
        if (m_state == Phonon::ErrorState || m_state == Phonon::PausedState) {
            return true;
        }
        Q_ASSERT(!m_mrl.isEmpty());
        /*if (m_mrl.isEmpty()) {
            kError(610) << "PauseCommand: m_mrl is empty. This should not happen.";
            error(Phonon::NormalError, i18n("Request to pause without media data"));
            return true;
        } */
        if (!m_stream) {
            QMutexLocker locker(&m_mutex);
            createStream();
            if (!m_stream) {
                qWarning() << "PauseCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, tr("Xine failed to create a stream."));
                return true;
            }
        }
        if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
            debug() << Q_FUNC_INFO << "calling xineOpen from PauseCommand";
            if (!xineOpen(Phonon::StoppedState)) {
                return true;
            }
        }
        internalPause();
        return true;
    case Event::StopCommand:
        ev->accept();
        if (m_state == Phonon::ErrorState || m_state == Phonon::LoadingState || m_state == Phonon::StoppedState) {
            return true;
        }
        Q_ASSERT(!m_mrl.isEmpty());
        /*if (m_mrl.isEmpty()) {
            kError(610) << "StopCommand: m_mrl is empty. This should not happen.";
            error(Phonon::NormalError, i18n("Request to stop without media data"));
            return true;
        } */
        if (!m_stream) {
            QMutexLocker locker(&m_mutex);
            createStream();
            if (!m_stream) {
                qWarning() << "StopCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, tr("Xine failed to create a stream."));
                return true;
            }
        }
        xine_stop(m_stream);
        changeState(Phonon::StoppedState);
        return true;
    case Event::UnloadCommand:
        ev->accept();
        if(m_event_queue) {
            xine_event_dispose_queue(m_event_queue);
            m_event_queue = 0;
        }
        if(m_stream) {
            xine_dispose(m_stream);
            m_stream = 0;
        }
        delete m_prefinishMarkTimer;
        m_prefinishMarkTimer = 0;
        if (m_nullAudioPort) {
            xine_close_audio_driver(m_xine, m_nullAudioPort);
            m_nullAudioPort = 0;
        }
        if (m_nullVideoPort) {
            xine_close_video_driver(m_xine, m_nullVideoPort);
            m_nullVideoPort = 0;
        }
        return true;
    case Event::SetTickInterval:
        ev->accept();
        {
            SetTickIntervalEvent *e = static_cast<SetTickIntervalEvent *>(ev);
            if (e->interval <= 0) {
                // disable ticks
                m_ticking = false;
                m_tickTimer.stop();
                //debug() << Q_FUNC_INFO << "tickTimer stopped.";
            } else {
                m_tickTimer.setInterval(e->interval);
                if (m_ticking == false && m_state == Phonon::PlayingState) {
                    m_tickTimer.start();
                    //debug() << Q_FUNC_INFO << "tickTimer started.";
                }
                m_ticking = true;
            }
        }
        return true;
    case Event::SetPrefinishMark:
        ev->accept();
        {
            SetPrefinishMarkEvent *e = static_cast<SetPrefinishMarkEvent *>(ev);
            m_prefinishMark = e->time;
            if (m_prefinishMark > 0) {
                updateTime();
                if (m_currentTime < m_totalTime - m_prefinishMark) { // not about to finish
                    m_prefinishMarkReachedNotEmitted = true;
                    if (m_state == Phonon::PlayingState) {
                        emitAboutToFinishIn(m_totalTime - m_prefinishMark - m_currentTime);
                    }
                }
            }
        }
        return true;
    case Event::SeekCommand:
        ev->accept();
        if (m_state == Phonon::ErrorState || !m_isSeekable) {
            return true;
        } else {
            SeekCommandEvent *e = static_cast<SeekCommandEvent *>(ev);
            if (m_lastSeekCommand != e) { // a newer SeekCommand is in the pipe, ignore this one
                return true;
            }
            switch(m_state) {
            case Phonon::PausedState:
            case Phonon::BufferingState:
            case Phonon::PlayingState:
                debug() << Q_FUNC_INFO << "seeking xine stream to " << e->time << "ms";
                // xine_trick_mode aborts :(
                //if (0 == xine_trick_mode(m_stream, XINE_TRICK_MODE_SEEK_TO_TIME, e->time)) {
                xine_play(m_stream, 0, e->time);

#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
                if (!m_useGaplessPlayback && m_transitionGap > 0) {
                    debug() << Q_FUNC_INFO << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
                    xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
                }
#endif // XINE_PARAM_DELAY_FINISHED_EVENT

                if (Phonon::PausedState == m_state) {
                    // go back to paused speed after seek
                    xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
                } else if (Phonon::PlayingState == m_state) {
                    gettimeofday(&m_lastTimeUpdate, 0);
                }
                //}
                break;
            case Phonon::StoppedState:
            case Phonon::ErrorState:
            case Phonon::LoadingState:
                return true; // cannot seek
            }
            m_currentTime = e->time;
            const int timeToSignal = m_totalTime - m_prefinishMark - e->time;
            if (m_prefinishMark > 0) {
                if (timeToSignal > 0) { // not about to finish
                    m_prefinishMarkReachedNotEmitted = true;
                    emitAboutToFinishIn(timeToSignal);
                } else if (m_prefinishMarkReachedNotEmitted) {
                    m_prefinishMarkReachedNotEmitted = false;
                    debug() << Q_FUNC_INFO << "emitting prefinishMarkReached(" << timeToSignal + m_prefinishMark << ")";
                    emit prefinishMarkReached(timeToSignal + m_prefinishMark);
                }
            }
        }
        return true;
    default:
        return QObject::event(ev);
    }
}

// called from main thread
void XineStream::closeBlocking()
{
    m_mutex.lock();
    m_closing = true;
    if (m_stream && xine_get_status(m_stream) != XINE_STATUS_IDLE) {
        // this event will call xine_close
        QCoreApplication::postEvent(this, new MrlChangedEvent(QByteArray(), StoppedState));

        // wait until the xine_close is done
        m_waitingForClose.wait(&m_mutex);
        //m_closing = false;
    }
    m_mutex.unlock();
}

// called from main thread
void XineStream::setError(Phonon::ErrorType type, const QString &reason)
{
    QCoreApplication::postEvent(this, new ErrorEvent(type, reason));
}

struct ReadLock
{
    ReadLock(QReadWriteLock &l) : lock(l) { lock.lockForRead(); }
    ~ReadLock() { lock.unlock(); }
    QReadWriteLock &lock;
};

// called from main thread
QString XineStream::errorString() const
{
    ReadLock lock(m_errorLock);
    return m_errorString;
}
// called from main thread
Phonon::ErrorType XineStream::errorType() const
{
    ReadLock lock(m_errorLock);
    return m_errorType;
}

QList<SubtitleDescription> XineStream::availableSubtitles() const
{
    uint hash = streamHash();
    QList<SubtitleDescription> subtitles;
    if (m_stream && m_mutex.tryLock(500)) {
        const int channels = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
        for(int index = 0; index < channels; index++) {
            subtitles << streamDescription<SubtitleDescription>(index, hash, SubtitleType, xine_get_spu_lang);
        }
        m_mutex.unlock();
    }
    return subtitles;
}

QList<AudioChannelDescription> XineStream::availableAudioChannels() const
{
    const uint hash = streamHash();
    QList<AudioChannelDescription> audios;
    if (m_stream && m_mutex.tryLock(500)) {
        if (m_stream) {
            const int channels = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL);
            for(int index = 0; index < channels; index++) {
                audios << streamDescription<AudioChannelDescription>(index, hash, AudioChannelType, xine_get_audio_lang);
            }
        }
        m_mutex.unlock();
    }
    return audios;
}

int XineStream::subtitlesSize() const
{
    int r = 0;
    if (m_stream && m_mutex.tryLock(500)) {
        if (m_stream) {
            r = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL);
        }
        m_mutex.unlock();
    }
    return r;
}

int XineStream::audioChannelsSize() const
{
    int r = 0;
    if (m_stream && m_mutex.tryLock(500)) {
        if (m_stream) {
            r = xine_get_stream_info(m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL);
        }
        m_mutex.unlock();
    }
    return r;
}

void XineStream::setCurrentAudioChannel(const AudioChannelDescription& streamDesc)
{
    xine_set_param(m_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, streamDesc.index() - streamHash());
}

void XineStream::setCurrentSubtitle(const SubtitleDescription& streamDesc)
{
    debug() << Q_FUNC_INFO << "setting the subtitle to: " << streamDesc.index();
    xine_set_param(m_stream, XINE_PARAM_SPU_CHANNEL, streamDesc.index() - streamHash());
}

uint XineStream::streamHash() const
{
    return qHash(m_mrl);
}

template<class S>
S XineStream::streamDescription(int index, uint hash, ObjectDescriptionType type, int(*get_xine_stream_text)(xine_stream_t *stream, int channel, char *lang)) const
{
    QByteArray lang;
    lang.resize(150);
    get_xine_stream_text(m_stream, index, lang.data());
    QHash<QByteArray, QVariant> properities;
    properities.insert("name", QString(lang));
    Backend::setObjectDescriptionProperities(type, index + hash, properities);
    return S(index + hash, properities);
}

AudioChannelDescription XineStream::currentAudioChannel() const
{
    AudioChannelDescription r;
    if (m_stream && m_mutex.tryLock(500)) {
        if (m_stream) {
            const int index = xine_get_param(m_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);
            r = streamDescription<AudioChannelDescription>(index, streamHash(), AudioChannelType, xine_get_audio_lang);
        }
        m_mutex.unlock();
    }
    return r;
}

SubtitleDescription XineStream::currentSubtitle() const
{
    SubtitleDescription r;
    if (m_stream && m_mutex.tryLock(500)) {
        if (m_stream) {
            const int index = xine_get_param(m_stream, XINE_PARAM_SPU_CHANNEL);
            r = streamDescription<SubtitleDescription>(index, streamHash(), SubtitleType, xine_get_spu_lang);
        }
        m_mutex.unlock();
    }
    return r;
}

xine_post_out_t *XineStream::audioOutputPort() const
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!m_stream) {
        return 0;
    }
    return xine_get_audio_source(m_stream);
}

xine_post_out_t *XineStream::videoOutputPort() const
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!m_stream) {
        return 0;
    }
    return xine_get_video_source(m_stream);
}

// called from main thread
void XineStream::setMrl(const QByteArray &mrl, StateForNewMrl sfnm)
{
    debug() << Q_FUNC_INFO << mrl << ", " << sfnm;
    QCoreApplication::postEvent(this, new MrlChangedEvent(mrl, sfnm));
}

// called from main thread
void XineStream::play()
{
    QCoreApplication::postEvent(this, new QEVENT(PlayCommand));
}

// called from main thread
void XineStream::pause()
{
    QCoreApplication::postEvent(this, new QEVENT(PauseCommand));
}

// called from main thread
void XineStream::stop()
{
    QCoreApplication::postEvent(this, new QEVENT(StopCommand));
}

void XineStream::unload()
{
    QCoreApplication::postEvent(this, new QEVENT(UnloadCommand));
}

// called from main thread
void XineStream::seek(qint64 time)
{
    m_lastSeekCommand = new SeekCommandEvent(time);
    QCoreApplication::postEvent(this, m_lastSeekCommand);
}

// xine thread
bool XineStream::updateTime()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!m_stream) {
        return false;
    }

    if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
        debug() << Q_FUNC_INFO << "calling xineOpen from ";
        if (!xineOpen(Phonon::StoppedState)) {
            return false;
        }
    }

    QMutexLocker locker(&m_updateTimeMutex);
    int newTotalTime;
    int newCurrentTime;
    if (xine_get_pos_length(m_stream, 0, &newCurrentTime, &newTotalTime) != 1) {
        //m_currentTime = -1;
        //m_totalTime = -1;
        //m_lastTimeUpdate.tv_sec = 0;
        return false;
    }
    if (newTotalTime != m_totalTime) {
        m_totalTime = newTotalTime;
        emit length(m_totalTime);
    }
    if (newCurrentTime <= 0) {
        // are we seeking? when xine seeks xine_get_pos_length returns 0 for m_currentTime
        //m_lastTimeUpdate.tv_sec = 0;
        // XineStream::currentTime will still return the old value counting with gettimeofday
        return false;
    }
    if (m_state == Phonon::PlayingState && m_currentTime != newCurrentTime) {
        gettimeofday(&m_lastTimeUpdate, 0);
    } else {
        m_lastTimeUpdate.tv_sec = 0;
    }
    m_currentTime = newCurrentTime;
    return true;
}

// xine thread
void XineStream::emitAboutToFinishIn(int timeToAboutToFinishSignal)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    //debug() << Q_FUNC_INFO << timeToAboutToFinishSignal;
    Q_ASSERT(m_prefinishMark > 0);
    if (!m_prefinishMarkTimer) {
        m_prefinishMarkTimer = new QTimer(this);
        //m_prefinishMarkTimer->setObjectName("prefinishMarkReached timer");
        Q_ASSERT(m_prefinishMarkTimer->thread() == XineThread::instance());
        m_prefinishMarkTimer->setSingleShot(true);
        connect(m_prefinishMarkTimer, SIGNAL(timeout()), SLOT(emitAboutToFinish()), Qt::DirectConnection);
    }
    timeToAboutToFinishSignal -= 400; // xine is not very accurate wrt time info, so better look too
                                      // often than too late
    if (timeToAboutToFinishSignal < 0) {
        timeToAboutToFinishSignal = 0;
    }
    //debug() << Q_FUNC_INFO << timeToAboutToFinishSignal;
    m_prefinishMarkTimer->start(timeToAboutToFinishSignal);
}

// xine thread
void XineStream::emitAboutToFinish()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    //debug() << Q_FUNC_INFO << m_prefinishMarkReachedNotEmitted << ", " << m_prefinishMark;
    if (m_prefinishMarkReachedNotEmitted && m_prefinishMark > 0) {
        updateTime();
        const int remainingTime = m_totalTime - m_currentTime;

        //debug() << Q_FUNC_INFO << remainingTime;
        if (remainingTime <= m_prefinishMark + 150) {
            m_prefinishMarkReachedNotEmitted = false;
            debug() << Q_FUNC_INFO << "emitting prefinishMarkReached(" << remainingTime << ")";
            emit prefinishMarkReached(remainingTime);
        } else {
            emitAboutToFinishIn(remainingTime - m_prefinishMark);
        }
    }
}

// xine thread
void XineStream::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (m_waitForPlayingTimerId == event->timerId()) {
        if (m_state != Phonon::BufferingState) {
            // the state has already changed somewhere else (probably from XineProgressEvents)
            killTimer(m_waitForPlayingTimerId);
            m_waitForPlayingTimerId = -1;
            return;
        }
        if (updateTime()) {
            changeState(Phonon::PlayingState);
            killTimer(m_waitForPlayingTimerId);
            m_waitForPlayingTimerId = -1;
        } else {
            if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
                changeState(Phonon::StoppedState);
                killTimer(m_waitForPlayingTimerId);
                m_waitForPlayingTimerId = -1;
            //} else {
                //debug() << Q_FUNC_INFO << "waiting";
            }
        }
    } else {
        QObject::timerEvent(event);
    }
}

// xine thread
void XineStream::emitTick()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (!updateTime()) {
        debug() << Q_FUNC_INFO << "no useful time information available. skipped.";
        return;
    }
    if (m_ticking) {
        //debug() << Q_FUNC_INFO << m_currentTime;
        emit tick(m_currentTime);
    }
    if (m_prefinishMarkReachedNotEmitted && m_prefinishMark > 0) {
        const int remainingTime = m_totalTime - m_currentTime;
        const int timeToAboutToFinishSignal = remainingTime - m_prefinishMark;
        if (timeToAboutToFinishSignal <= m_tickTimer.interval()) { // about to finish
            if (timeToAboutToFinishSignal > 100) {
                emitAboutToFinishIn(timeToAboutToFinishSignal);
            } else {
                m_prefinishMarkReachedNotEmitted = false;
                debug() << Q_FUNC_INFO << "emitting prefinishMarkReached(" << remainingTime << ")";
                emit prefinishMarkReached(remainingTime);
            }
        }
    }
}

// xine thread
void XineStream::getStartTime()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
//X     if (m_startTime == -1 || m_startTime == 0) {
//X         int total;
//X         if (xine_get_pos_length(m_stream, 0, &m_startTime, &total) == 1) {
//X             if(total > 0 && m_startTime < total && m_startTime >= 0)
//X                 m_startTime = -1;
//X         } else {
//X             m_startTime = -1;
//X         }
//X     }
//X     if (m_startTime == -1 || m_startTime == 0) {
//X         QTimer::singleShot(30, this, SLOT(getStartTime()));
//X     }
}

void XineStream::internalPause()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (m_state == Phonon::PlayingState || m_state == Phonon::BufferingState) {
        xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
        changeState(Phonon::PausedState);
    } else {
        xine_play(m_stream, 0, 0);
        xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
        changeState(Phonon::PausedState);
    }
}

void XineStream::internalPlay()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    xine_play(m_stream, 0, 0);

#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
    if (!m_useGaplessPlayback && m_transitionGap > 0) {
        debug() << Q_FUNC_INFO << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
        xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
    }
#endif // XINE_PARAM_DELAY_FINISHED_EVENT

    if (updateTime()) {
        changeState(Phonon::PlayingState);
    } else {
        changeState(Phonon::BufferingState);
        m_waitForPlayingTimerId = startTimer(50);
    }
}

void XineStream::setMrlInternal(const QByteArray &newMrl)
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    if (newMrl != m_mrl) {
        if (m_mrl.startsWith("kbytestream:/")) {
            Q_ASSERT(m_byteStream);
            Q_ASSERT(ByteStream::fromMrl(m_mrl) == m_byteStream.data());
            m_byteStream.reset();
        }
        m_mrl = newMrl;
        if (m_mrl.startsWith("kbytestream:/")) {
            Q_ASSERT(m_byteStream.data() == 0);
            m_byteStream = ByteStream::fromMrl(m_mrl);
            Q_ASSERT(m_byteStream);
        }
    }
}

void XineStream::handleDownstreamEvent(Event *e)
{
    emit downstreamEvent(e);
}

} // namespace Xine
} // namespace Phonon

#include "xinestream.moc"

// vim: sw=4 ts=4
