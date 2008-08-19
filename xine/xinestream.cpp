/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>
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
#include <QEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QVarLengthArray>

#include <klocale.h>
#include <kurl.h>

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
    //kDebug(610) << "Xine event: " << xineEvent->type << QByteArray((char *)xineEvent->data, xineEvent->data_length);

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
        kDebug(610) << "XINE_EVENT_UI_CHANNELS_CHANGED";
        {
            QCoreApplication::postEvent(xs, new QEVENT(UiChannelsChanged));
        }
        break;
    case XINE_EVENT_UI_MESSAGE:             /* message (dialog) for the ui to display */
        {
            kDebug(610) << "XINE_EVENT_UI_MESSAGE";
            const xine_ui_message_data_t *message = static_cast<xine_ui_message_data_t *>(xineEvent->data);
            if (message->type == XINE_MSG_AUDIO_OUT_UNAVAILABLE) {
                kDebug(610) << "XINE_MSG_AUDIO_OUT_UNAVAILABLE";
                // we don't know for sure which AudioOutput failed. but the one without any
                // capabilities must be the guilty one
                xs->handleDownstreamEvent(new QEVENT(AudioDeviceFailed));
            }
        }
        break;
    case XINE_EVENT_FRAME_FORMAT_CHANGE:    /* e.g. aspect ratio change during dvd playback */
        kDebug(610) << "XINE_EVENT_FRAME_FORMAT_CHANGE";
        {
            xine_format_change_data_t *data = static_cast<xine_format_change_data_t *>(xineEvent->data);
            xs->handleDownstreamEvent(new FrameFormatChangeEvent(data->width, data->height, data->aspect, data->pan_scan));
        }
        break;
    case XINE_EVENT_AUDIO_LEVEL:            /* report current audio level (l/r/mute) */
        kDebug(610) << "XINE_EVENT_AUDIO_LEVEL";
        break;
    case XINE_EVENT_QUIT:                   /* last event sent when stream is disposed */
        kDebug(610) << "XINE_EVENT_QUIT";
        break;
    case XINE_EVENT_UI_NUM_BUTTONS:         /* number of buttons for interactive menus */
        kDebug(610) << "XINE_EVENT_UI_NUM_BUTTONS";
        break;
    case XINE_EVENT_DROPPED_FRAMES:         /* number of dropped frames is too high */
        kDebug(610) << "XINE_EVENT_DROPPED_FRAMES";
        break;
    case XINE_EVENT_MRL_REFERENCE_EXT:      /* demuxer->frontend: MRL reference(s) for the real stream */
        {
            xine_mrl_reference_data_ext_t *reference = static_cast<xine_mrl_reference_data_ext_t *>(xineEvent->data);
            kDebug(610) << "XINE_EVENT_MRL_REFERENCE_EXT: " << reference->alternative
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
    m_deinterlacer(0),
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
    if (m_deinterlacer) {
        xine_post_dispose(m_xine, m_deinterlacer);
    }
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
        kDebug(610) << "faked xine_open failed for m_mrl =" << m_mrl.constData();
        error(Phonon::NormalError, i18n("Cannot open media data at '<i>%1</i>'", m_mrl.constData()));
        return false;
    }
#endif

    // xine_open can call functions from ByteStream which will block waiting for data.
    //kDebug(610) << "xine_open(" << m_mrl.constData() << ")";
    if (xine_open(m_stream, m_mrl.constData()) == 0) {
        kDebug(610) << "xine_open failed for m_mrl =" << m_mrl.constData();
        switch (xine_get_error(m_stream)) {
        case XINE_ERROR_NONE:
            // hmm?
            abort();
        case XINE_ERROR_NO_INPUT_PLUGIN:
            error(Phonon::NormalError, i18n("Cannot find input plugin for MRL [%1]", m_mrl.constData()));
            break;
        case XINE_ERROR_NO_DEMUX_PLUGIN:
            if (m_mrl.startsWith("kbytestream:/")) {
                error(Phonon::FatalError, i18n("Cannot find demultiplexer plugin for the given media data"));
            } else {
                error(Phonon::FatalError, i18n("Cannot find demultiplexer plugin for MRL [%1]", m_mrl.constData()));
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
    kDebug(610) << "xine_open succeeded for m_mrl =" << m_mrl.constData();
    const bool needDeinterlacer =
        (m_mrl.startsWith("dvd:/") && Backend::deinterlaceDVD()) ||
        (m_mrl.startsWith("vcd:/") && Backend::deinterlaceVCD()) ||
        (m_mrl.startsWith("file:/") && Backend::deinterlaceFile());
    if (m_deinterlacer) {
        if (!needDeinterlacer) {
            xine_post_dispose(m_xine, m_deinterlacer);
            m_deinterlacer = 0;
        }
    } else if (needDeinterlacer) {
        xine_video_port_t *videoPort = 0;
        Q_ASSERT(m_mediaObject);
        QSet<SinkNode *> sinks = m_mediaObject->sinks();
        foreach (SinkNode *sink, sinks) {
            Q_ASSERT(sink->threadSafeObject());
            if (sink->threadSafeObject()->videoPort()) {
                Q_ASSERT(videoPort == 0);
                videoPort = sink->threadSafeObject()->videoPort();
            }
        }
        if (!videoPort) {
            kDebug(610) << "creating xine_stream with null video port";
            videoPort = nullVideoPort();
        }
        m_deinterlacer = xine_post_init(m_xine, "tvtime", 1, 0, &videoPort);
        if (m_deinterlacer) {
            // set method
            xine_post_in_t *paraInput = xine_post_input(m_deinterlacer, "parameters");
            Q_ASSERT(paraInput);
            Q_ASSERT(paraInput->data);
            xine_post_api_t *api = reinterpret_cast<xine_post_api_t *>(paraInput->data);
            xine_post_api_descr_t *desc = api->get_param_descr();
            char *pluginParams = static_cast<char *>(malloc(desc->struct_size));
            api->get_parameters(m_deinterlacer, pluginParams);
            for (int i = 0; desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i) {
                xine_post_api_parameter_t &p = desc->parameter[i];
                if (p.type == POST_PARAM_TYPE_INT && 0 == strcmp(p.name, "method")) {
                    int *value = reinterpret_cast<int *>(pluginParams + p.offset);
                    *value = Backend::deinterlaceMethod();
                    break;
                }
            }
            api->set_parameters(m_deinterlacer, pluginParams);
            free(pluginParams);

            // connect to xine_stream_t
            xine_post_in_t *x = xine_post_input(m_deinterlacer, "video");
            Q_ASSERT(x);
            xine_post_out_t *videoOutputPort = xine_get_video_source(m_stream);
            Q_ASSERT(videoOutputPort);
            xine_post_wire(videoOutputPort, x);
        }
    }

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
            kDebug(610) << "waitcondition timed out";
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
            kDebug(610) << "waitcondition timed out";
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
            kDebug(610) << "calling xineOpen from ";
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
            kDebug(610) << "available titles changed: " << availableTitles;
            m_availableTitles = availableTitles;
            emit availableTitlesChanged(m_availableTitles);
        }
        if (m_availableChapters != availableChapters) {
            kDebug(610) << "available chapters changed: " << availableChapters;
            m_availableChapters = availableChapters;
            emit availableChaptersChanged(m_availableChapters);
        }
        if (m_availableAngles != availableAngles) {
            kDebug(610) << "available angles changed: " << availableAngles;
            m_availableAngles = availableAngles;
            emit availableAnglesChanged(m_availableAngles);
        }
        if (m_availableSubtitles != availableSubtitles) {
            kDebug(610) << "available angles changed: " << availableSubtitles;
            m_availableSubtitles = availableSubtitles;
            emit availableSubtitlesChanged();
        }
        if (m_availableAudioChannels != availableAudioChannels) {
            kDebug(610) << "available angles changed: " << availableAudioChannels;
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
    //kDebug(610) << "AudioPort.xinePort() = " << m_audioPort.xinePort();
    xine_audio_port_t *audioPort = 0;
    xine_video_port_t *videoPort = 0;
    Q_ASSERT(m_mediaObject);
    QSet<SinkNode *> sinks = m_mediaObject->sinks();
    kDebug(610) << "MediaObject is connected to " << sinks.size() << " nodes";
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
        kDebug(610) << "creating xine_stream with null audio port";
        audioPort = nullAudioPort();
    }
    if (!videoPort) {
        kDebug(610) << "creating xine_stream with null video port";
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
        kDebug(610) << "XINE_PARAM_EARLY_FINISHED_EVENT: 1";
        xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);
#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
    } else if (m_transitionGap > 0) {
        kDebug(610) << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
        xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
#endif // XINE_PARAM_DELAY_FINISHED_EVENT
    } else {
        kDebug(610) << "XINE_PARAM_EARLY_FINISHED_EVENT: 0";
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
    kDebug(610);
    m_portMutex.lock();

    // schedule m_stream rewiring
    QCoreApplication::postEvent(this, new QEVENT(RewireVideoToNull));
    kDebug(610) << "waiting for rewire";
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
void XineStream::gaplessSwitchTo(const KUrl &url)
{
    gaplessSwitchTo(url.url().toUtf8());
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
            //kDebug(610) << "tickTimer started.";
        }
        if (m_prefinishMark > 0) {
            emitAboutToFinish();
        }
    } else if (oldstate == Phonon::PlayingState) {
        m_tickTimer.stop();
        //kDebug(610) << "tickTimer stopped.";
        m_prefinishMarkReachedNotEmitted = true;
        if (m_prefinishMarkTimer) {
            m_prefinishMarkTimer->stop();
        }
    }
    if (newstate == Phonon::ErrorState) {
        kDebug(610) << "reached error state";// from: " << kBacktrace();
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

// xine thread
void XineStream::updateMetaData()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    QMultiMap<QString, QString> metaDataMap;
    metaDataMap.insert(QLatin1String("TITLE"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_TITLE)));
    metaDataMap.insert(QLatin1String("ARTIST"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_ARTIST)));
    metaDataMap.insert(QLatin1String("GENRE"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_GENRE)));
    metaDataMap.insert(QLatin1String("ALBUM"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_ALBUM)));
    metaDataMap.insert(QLatin1String("DATE"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_YEAR)));
    metaDataMap.insert(QLatin1String("TRACKNUMBER"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_TRACK_NUMBER)));
    metaDataMap.insert(QLatin1String("DESCRIPTION"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_COMMENT)));
    metaDataMap.insert(QLatin1String("MUSICBRAINZ_DISCID"),
            QString::fromUtf8(xine_get_meta_info(m_stream, XINE_META_INFO_CDINDEX_DISCID)));
    if(metaDataMap == m_metaDataMap)
        return;
    m_metaDataMap = metaDataMap;
    //kDebug(610) << "emitting metaDataChanged(" << m_metaDataMap << ")";
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
    kDebug(610) << type << string;
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
                kDebug(610) << "####################### ignoring Event: " << eventName;
            }
            return QObject::event(ev);
        }
    }
    if (eventName) {
        if (static_cast<int>(ev->type()) == Event::Progress) {
            ProgressEvent *e = static_cast<ProgressEvent *>(ev);
            kDebug(610) << "################################ Event: " << eventName << ": " << e->percent;
        } else {
            kDebug(610) << "################################ Event: " << eventName;
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
                kDebug(610) << "available titles changed: " << availableTitles;
                m_availableTitles = availableTitles;
                emit availableTitlesChanged(m_availableTitles);
            }
            if (m_availableChapters != availableChapters) {
                kDebug(610) << "available chapters changed: " << availableChapters;
                m_availableChapters = availableChapters;
                emit availableChaptersChanged(m_availableChapters);
            }
            if (m_availableAngles != availableAngles) {
                kDebug(610) << "available angles changed: " << availableAngles;
                m_availableAngles = availableAngles;
                emit availableAnglesChanged(m_availableAngles);
            }

            {
                int availableSubtitles = subtitlesSize();
                if(availableSubtitles != m_availableSubtitles)
                {
                    kDebug(610) << "available subtitles changed: " << availableSubtitles;
                    m_availableSubtitles = availableSubtitles;
                    emit availableSubtitlesChanged();
                }
            }
            {
                int availableAudioChannels = audioChannelsSize();
                if(availableAudioChannels != m_availableAudioChannels)
                {
                    kDebug(610) << "available audio channels changed: " << availableAudioChannels;
                    m_availableAudioChannels = availableAudioChannels;
                    emit availableAudioChannelsChanged();
                }
            }

            int currentTitle   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER);
            int currentChapter = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER);
            int currentAngle   = xine_get_stream_info(m_stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER);
            if (currentAngle != m_currentAngle) {
                kDebug(610) << "current angle changed: " << currentAngle;
                m_currentAngle = currentAngle;
                emit angleChanged(m_currentAngle);
            }
            if (currentChapter != m_currentChapter) {
                kDebug(610) << "current chapter changed: " << currentChapter;
                m_currentChapter = currentChapter;
                emit chapterChanged(m_currentChapter);
            }
            if (currentTitle != m_currentTitle) {
                kDebug(610) << "current title changed: " << currentTitle;
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
        kDebug(610) << "MediaFinishedEvent m_useGaplessPlayback = " << m_useGaplessPlayback;
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
                kDebug(610) << "no GaplessSwitch";
            } else {
                setMrlInternal(e->mrl);
                kDebug(610) << "GaplessSwitch new m_mrl =" << m_mrl.constData();
            }
            if (e->mrl.isEmpty() || m_closing) {
                xine_set_param(m_stream, XINE_PARAM_GAPLESS_SWITCH, 0);
                m_mutex.unlock();
                playbackFinished();
                return true;
            }
            if (!xine_open(m_stream, m_mrl.constData())) {
                kWarning(610) << "xine_open for gapless playback failed!";
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
            kDebug(610) << "emit bufferStatus(" << e->percent << ")";
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
            kDebug(610) << "taking snapshot of" << w << h;
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
                    kDebug(610) << "got a YUY2 snapshot";
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
                    kDebug(610) << "got a YV12 snapshot";
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
                handleDownstreamEvent(new SnapshotReadyEvent(qimg));
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
                    kError(610) << "MrlChangedEvent: createStream didn't create a stream. This should not happen.";
                    error(Phonon::FatalError, i18n("Xine failed to create a stream."));
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
                kDebug(610) << "MrlChanged: don't call xineOpen. m_closing =" << m_closing << ", m_mrl =" << m_mrl.constData();
                m_waitingForClose.wakeAll();
            } else {
                kDebug(610) << "calling xineOpen from MrlChanged";
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
                kDebug(610) << "XINE_PARAM_EARLY_FINISHED_EVENT: 1";
                xine_set_param(m_stream, XINE_PARAM_EARLY_FINISHED_EVENT, 1);
#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
            } else if (m_transitionGap > 0) {
                kDebug(610) << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
                xine_set_param(m_stream, XINE_PARAM_DELAY_FINISHED_EVENT, m_transitionGap);
#endif // XINE_PARAM_DELAY_FINISHED_EVENT
            } else {
                kDebug(610) << "XINE_PARAM_EARLY_FINISHED_EVENT: 0";
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
            kDebug(610) << "rewiring ports";
            xine_post_out_t *videoSource = xine_get_video_source(m_stream);
            xine_video_port_t *videoPort = nullVideoPort();
            xine_post_wire_video_port(videoSource, videoPort);
            m_waitingForRewire.wakeAll();
        }
        return true;
    case Event::PlayCommand:
        ev->accept();
        if (m_mediaObject->sinks().isEmpty()) {
            kWarning(610) << "request to play a stream, but no valid audio/video outputs are given/available";
            error(Phonon::FatalError, i18n("Playback failed because no valid audio or video outputs are available"));
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
                kError(610) << "PlayCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, i18n("Xine failed to create a stream."));
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
                kDebug(610) << "calling xineOpen from PlayCommand";
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
                kError(610) << "PauseCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, i18n("Xine failed to create a stream."));
                return true;
            }
        }
        if (xine_get_status(m_stream) == XINE_STATUS_IDLE) {
            kDebug(610) << "calling xineOpen from PauseCommand";
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
                kError(610) << "StopCommand: createStream didn't create a stream. This should not happen.";
                error(Phonon::FatalError, i18n("Xine failed to create a stream."));
                return true;
            }
        }
        xine_stop(m_stream);
        changeState(Phonon::StoppedState);
        return true;
    case Event::SetTickInterval:
        ev->accept();
        {
            SetTickIntervalEvent *e = static_cast<SetTickIntervalEvent *>(ev);
            if (e->interval <= 0) {
                // disable ticks
                m_ticking = false;
                m_tickTimer.stop();
                //kDebug(610) << "tickTimer stopped.";
            } else {
                m_tickTimer.setInterval(e->interval);
                if (m_ticking == false && m_state == Phonon::PlayingState) {
                    m_tickTimer.start();
                    //kDebug(610) << "tickTimer started.";
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
                kDebug(610) << "seeking xine stream to " << e->time << "ms";
                // xine_trick_mode aborts :(
                //if (0 == xine_trick_mode(m_stream, XINE_TRICK_MODE_SEEK_TO_TIME, e->time)) {
                xine_play(m_stream, 0, e->time);

#ifdef XINE_PARAM_DELAY_FINISHED_EVENT
                if (!m_useGaplessPlayback && m_transitionGap > 0) {
                    kDebug(610) << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
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
                    kDebug(610) << "emitting prefinishMarkReached(" << timeToSignal + m_prefinishMark << ")";
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
    if( !m_stream )
        return subtitles;
    const int channels = subtitlesSize();
    for( int index = 0; index < channels; index++ )
    {
        subtitles << streamDescription<SubtitleDescription>( index, hash, SubtitleType, xine_get_spu_lang );
    }
    return subtitles;
}

QList<AudioChannelDescription> XineStream::availableAudioChannels() const
{
    const uint hash = streamHash();
    QList<AudioChannelDescription> audios;
    if( !m_stream )
        return audios;
    const int channels = audioChannelsSize();
    for( int index = 0; index < channels; index++ )
    {
        audios << streamDescription<AudioChannelDescription>( index, hash, AudioChannelType, xine_get_audio_lang );
    }
    return audios;
}

int XineStream::subtitlesSize() const
{
    return xine_get_stream_info( m_stream, XINE_STREAM_INFO_MAX_SPU_CHANNEL );
}

int XineStream::audioChannelsSize() const
{
    return xine_get_stream_info( m_stream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL );
}

void XineStream::setCurrentAudioChannel(const AudioChannelDescription& streamDesc)
{
    xine_set_param( m_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, streamDesc.index() - streamHash() );
}

void XineStream::setCurrentSubtitle(const SubtitleDescription& streamDesc)
{
    kDebug() << "setting the subtitle to: " << streamDesc.index();
    xine_set_param( m_stream, XINE_PARAM_SPU_CHANNEL, streamDesc.index() - streamHash() );
}

uint XineStream::streamHash() const
{
    return qHash( m_mrl );
}

template<class S>
S XineStream::streamDescription(int index, uint hash, ObjectDescriptionType type, int(*get_xine_stream_text)(xine_stream_t *stream, int channel, char *lang)) const
{
    QByteArray lang;
    lang.resize( 150 );
    get_xine_stream_text( m_stream, index, lang.data() );
    QHash<QByteArray, QVariant> properities;
    properities.insert( "name", QString( lang ) );
    Backend::setObjectDescriptionProperities( type, index + hash, properities );
    return S( index + hash, properities );
}

AudioChannelDescription XineStream::currentAudioChannel() const
{
    const int index = xine_get_param( m_stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL );
    return streamDescription<AudioChannelDescription>( index, streamHash(), AudioChannelType, xine_get_audio_lang );
}

SubtitleDescription XineStream::currentSubtitle() const
{
    int index = xine_get_param( m_stream, XINE_PARAM_SPU_CHANNEL );
    return streamDescription<SubtitleDescription>( index, streamHash(), SubtitleType, xine_get_spu_lang );
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
    if (m_deinterlacer) {
        return xine_post_output(m_deinterlacer, "deinterlaced video");
    }
    return xine_get_video_source(m_stream);
}

// called from main thread
void XineStream::setUrl(const KUrl &url)
{
    setMrl(url.url().toUtf8());
}

// called from main thread
void XineStream::setMrl(const QByteArray &mrl, StateForNewMrl sfnm)
{
    kDebug(610) << mrl << ", " << sfnm;
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
        kDebug(610) << "calling xineOpen from ";
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
    //kDebug(610) << timeToAboutToFinishSignal;
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
    //kDebug(610) << timeToAboutToFinishSignal;
    m_prefinishMarkTimer->start(timeToAboutToFinishSignal);
}

// xine thread
void XineStream::emitAboutToFinish()
{
    Q_ASSERT(QThread::currentThread() == XineThread::instance());
    //kDebug(610) << m_prefinishMarkReachedNotEmitted << ", " << m_prefinishMark;
    if (m_prefinishMarkReachedNotEmitted && m_prefinishMark > 0) {
        updateTime();
        const int remainingTime = m_totalTime - m_currentTime;

        //kDebug(610) << remainingTime;
        if (remainingTime <= m_prefinishMark + 150) {
            m_prefinishMarkReachedNotEmitted = false;
            kDebug(610) << "emitting prefinishMarkReached(" << remainingTime << ")";
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
                //kDebug(610) << "waiting";
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
        kDebug(610) << "no useful time information available. skipped.";
        return;
    }
    if (m_ticking) {
        //kDebug(610) << m_currentTime;
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
                kDebug(610) << "emitting prefinishMarkReached(" << remainingTime << ")";
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
        kDebug(610) << "XINE_PARAM_DELAY_FINISHED_EVENT:" << m_transitionGap;
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
