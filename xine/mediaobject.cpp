/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2008 Ian Monroe <imonroe@kde.org>

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

#include "mediaobject.h"

#include "bytestream.h"

#include <QEvent>
#include <QFile>
#include <QVector>
#include <QByteArray>
#include <QStringList>
#include <QMultiMap>
#include <QtDebug>
#include <QMetaType>

#include <kdebug.h>
#include <klocale.h>

#include <cmath>
#include "xinethread.h"
#include "sinknode.h"
#include "videowidget.h"
#include "events.h"

static const char *const green  = "\033[1;40;32m";
static const char *const blue   = "\033[1;40;34m";
static const char *const normal = "\033[0m";

Q_DECLARE_METATYPE(QVariant)

namespace Phonon
{
namespace Xine
{
MediaObject::MediaObject(QObject *parent)
    : QObject(parent),
    SourceNode(XineThread::newStream()),
    m_state(Phonon::LoadingState),
    m_stream(static_cast<XineStream *>(SourceNode::threadSafeObject().data())),
    m_currentTitle(1),
    m_transitionTime(0),
    m_autoplayTitles(true),
    m_fakingBuffering(false),
    m_shouldFakeBufferingOnPlay(true)
{
    m_stream->setMediaObject(this);
    m_stream->useGaplessPlayback(true);

    qRegisterMetaType<QMultiMap<QString,QString> >("QMultiMap<QString,QString>");
    connect(m_stream, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            SLOT(handleStateChange(Phonon::State, Phonon::State)));
    connect(m_stream, SIGNAL(metaDataChanged(const QMultiMap<QString, QString> &)),
            SIGNAL(metaDataChanged(const QMultiMap<QString, QString> &)));
    connect(m_stream, SIGNAL(seekableChanged(bool)), SIGNAL(seekableChanged(bool)));
    connect(m_stream, SIGNAL(hasVideoChanged(bool)), SIGNAL(hasVideoChanged(bool)));
    connect(m_stream, SIGNAL(hasVideoChanged(bool)), SLOT(handleHasVideoChanged(bool)));
    connect(m_stream, SIGNAL(bufferStatus(int)), SIGNAL(bufferStatus(int)));
    connect(m_stream, SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
    connect(m_stream, SIGNAL(availableSubtitlesChanged()), SIGNAL(availableSubtitlesChanged()));
    connect(m_stream, SIGNAL(availableAudioChannelsChanged()), SIGNAL(availableAudioChannelsChanged()));
    connect(m_stream, SIGNAL(availableChaptersChanged(int)), SIGNAL(availableChaptersChanged(int)));
    connect(m_stream, SIGNAL(chapterChanged(int)), SIGNAL(chapterChanged(int)));
    connect(m_stream, SIGNAL(availableAnglesChanged(int)), SIGNAL(availableAnglesChanged(int)));
    connect(m_stream, SIGNAL(angleChanged(int)), SIGNAL(angleChanged(int)));
    connect(m_stream, SIGNAL(finished()), SLOT(handleFinished()), Qt::QueuedConnection);
    connect(m_stream, SIGNAL(length(qint64)), SIGNAL(totalTimeChanged(qint64)), Qt::QueuedConnection);
    connect(m_stream, SIGNAL(prefinishMarkReached(qint32)), SIGNAL(prefinishMarkReached(qint32)), Qt::QueuedConnection);
    connect(m_stream, SIGNAL(availableTitlesChanged(int)), SLOT(handleAvailableTitlesChanged(int)));
    connect(m_stream, SIGNAL(needNextUrl()), SLOT(needNextUrl()));
    connect(m_stream, SIGNAL(downstreamEvent(Event *)), SLOT(downstreamEvent(Event *)));

    qRegisterMetaType<QVariant>();
    connect(m_stream, SIGNAL(hackSetProperty(const char *, const QVariant &)), SLOT(syncHackSetProperty(const char *, const QVariant &)), Qt::QueuedConnection);
}

void MediaObject::syncHackSetProperty(const char *name, const QVariant &val)
{
    if (parent()) {
        parent()->setProperty(name, val);
    }
}

MediaObject::~MediaObject()
{
    if (m_bytestream) {
        // ByteStream must be stopped before calling XineStream::closeBlocking to avoid deadlocks
        // when closeBlocking waits for data in the ByteStream that will never arrive since the main
        // thread is blocking
        m_bytestream->stop();
        // don't delete m_bytestream - the xine input plugin owns it
    }
    m_stream->closeBlocking();
}

State MediaObject::state() const
{
    return m_state;
}

bool MediaObject::hasVideo() const
{
    return m_stream->hasVideo();
}

MediaStreamTypes MediaObject::outputMediaStreamTypes() const
{
    return Phonon::Xine::Audio | Phonon::Xine::Video;
}

bool MediaObject::isSeekable() const
{
    return m_stream->isSeekable();
}

qint64 MediaObject::currentTime() const
{
    //kDebug(610) << kBacktrace();
    switch(m_stream->state()) {
    case Phonon::PausedState:
    case Phonon::BufferingState:
    case Phonon::PlayingState:
        return m_stream->currentTime();
    case Phonon::StoppedState:
    case Phonon::LoadingState:
        return 0;
    case Phonon::ErrorState:
        break;
    }
    return -1;
}

qint64 MediaObject::totalTime() const
{
    const qint64 ret = m_stream->totalTime();
    //kDebug(610) << "returning " << ret;
    return ret;
}

qint64 MediaObject::remainingTime() const
{
    switch(m_stream->state()) {
    case Phonon::PausedState:
    case Phonon::BufferingState:
    case Phonon::PlayingState:
        {
            const qint64 ret = m_stream->remainingTime();
            //kDebug(610) << "returning " << ret;
            return ret;
        }
        break;
    case Phonon::StoppedState:
    case Phonon::LoadingState:
        //kDebug(610) << "returning 0";
        return 0;
    case Phonon::ErrorState:
        break;
    }
    //kDebug(610) << "returning -1";
    return -1;
}

qint32 MediaObject::tickInterval() const
{
    return m_tickInterval;
}

void MediaObject::setTickInterval(qint32 newTickInterval)
{
    m_tickInterval = newTickInterval;
    m_stream->setTickInterval(m_tickInterval);
}

void MediaObject::play()
{
    kDebug(610) << green << "PLAY" << normal;
    m_stream->play();
    if (m_shouldFakeBufferingOnPlay || m_state == Phonon::StoppedState || m_state == Phonon::LoadingState || m_state == Phonon::PausedState) {
        m_shouldFakeBufferingOnPlay = false;
        startToFakeBuffering();
    }
}

void MediaObject::pause()
{
    m_stream->pause();
}

void MediaObject::stop()
{
    //if (m_state == Phonon::PlayingState || m_state == Phonon::PausedState || m_state == Phonon::BufferingState) {
        m_stream->stop();
    //}
}

void MediaObject::seek(qint64 time)
{
    //kDebug(610) << time;
    m_stream->seek(time);
}

QString MediaObject::errorString() const
{
    return m_stream->errorString();
}

Phonon::ErrorType MediaObject::errorType() const
{
    return m_stream->errorType();
}

void MediaObject::startToFakeBuffering()
{
    kDebug(610) << blue << "start faking" << normal;
    m_fakingBuffering = true;
    if (m_state == Phonon::BufferingState) {
        return;
    } else if (m_state == Phonon::PlayingState) {
        // next time we reach StoppedState from LoadingState go right into BufferingState
        return;
    }

    kDebug(610) << "fake state change: reached BufferingState after " << m_state;

    Phonon::State oldstate = m_state;
    m_state = Phonon::BufferingState;

    emit stateChanged(Phonon::BufferingState, oldstate);
}

void MediaObject::handleStateChange(Phonon::State newstate, Phonon::State oldstate)
{
    if (m_state == newstate && m_state == BufferingState) {
        m_fakingBuffering = false;
        kDebug(610) << blue << "end faking" << normal;
        // BufferingState -> BufferingState, nothing to do
        return;
    } else if (m_state != oldstate) {
        // m_state == oldstate always, except when faking buffering:
        Q_ASSERT(m_fakingBuffering);

        // so we're faking BufferingState, then m_state must be in BufferingState
        Q_ASSERT(m_state == BufferingState);
        if (newstate == PlayingState || newstate == ErrorState) {
            m_fakingBuffering = false;
            kDebug(610) << blue << "end faking" << normal;
            oldstate = m_state;
        } else {
            // we're faking BufferingState and stay there until we either reach BufferingState,
            // PlayingState or ErrorState
            return;
        }
    } else if (oldstate == LoadingState && newstate == StoppedState && m_fakingBuffering) {
        newstate = BufferingState;
    }
    m_state = newstate;

    kDebug(610) << "reached " << newstate << " after " << oldstate;
    emit stateChanged(newstate, oldstate);
}
void MediaObject::handleFinished()
{
    kDebug(610) << "emit finished()";
    emit finished();
}

MediaSource MediaObject::source() const
{
    //kDebug(610);
    return m_mediaSource;
}

qint32 MediaObject::prefinishMark() const
{
    //kDebug(610);
    return m_prefinishMark;
}

qint32 MediaObject::transitionTime() const
{
    return m_transitionTime;
}

void MediaObject::setTransitionTime(qint32 newTransitionTime)
{
    if (m_transitionTime != newTransitionTime) {
        m_transitionTime = newTransitionTime;
        if (m_transitionTime == 0) {
            m_stream->useGaplessPlayback(true);
        } else if (m_transitionTime > 0) {
            m_stream->useGapOf((newTransitionTime + 50) / 100); // xine-lib provides a resolution of 1/10s
        } else {
            // TODO: a crossfade of milliseconds milliseconds
            m_stream->useGaplessPlayback(true);
        }
    }
}

void MediaObject::setNextSource(const MediaSource &source)
{
    if (m_transitionTime < 0) {
        kError(610) << "crossfades are not supported with the xine backend";
    } else if (m_transitionTime > 0) {
        if (source.type() == MediaSource::Invalid) {
            QMetaObject::invokeMethod(m_stream, "playbackFinished", Qt::QueuedConnection);
        } else {
            setSourceInternal(source, HardSwitch);
            play();
        }
        return;
    }
    if (source.type() == MediaSource::Invalid) {
        // the frontend is telling us that the play-queue is empty, so stop waiting for a new MRL
        // for gapless playback
        m_stream->gaplessSwitchTo(QByteArray());
        return;
    }
    setSourceInternal(source, GaplessSwitch);
}

void MediaObject::setSource(const MediaSource &source)
{
    setSourceInternal(source, HardSwitch);
}

void MediaObject::setSourceInternal(const MediaSource &source, HowToSetTheUrl how)
{
    //kDebug(610);
    m_titles.clear();
    m_mediaSource = source;

    switch (source.type()) {
    case MediaSource::Invalid:
        stop();
        break;
    case MediaSource::LocalFile:
    case MediaSource::Url:
        if (source.url().scheme() == QLatin1String("kbytestream")) {
            m_mediaSource = MediaSource();
            kError(610) << "do not ever use kbytestream:/ URLs with MediaObject!";
            m_shouldFakeBufferingOnPlay = false;
            m_stream->setMrl(QByteArray());
            m_stream->setError(Phonon::NormalError, i18n("Cannot open media data at '<i>%1</i>'", source.url().toString(QUrl::RemovePassword)));
            return;
        }
        switch (how) {
        case GaplessSwitch:
            m_stream->gaplessSwitchTo(source.url());
            break;
        case HardSwitch:
            m_shouldFakeBufferingOnPlay = true;
            m_stream->setUrl(source.url());
            break;
        }
        break;
    case MediaSource::Disc:
        {
            m_mediaDevice = QFile::encodeName(source.deviceName());
            if (!m_mediaDevice.isEmpty() && !m_mediaDevice.startsWith('/')) {
                kError(610) << "mediaDevice '" << m_mediaDevice << "' has to be an absolute path - starts with a /";
                m_mediaDevice.clear();
            }
            m_mediaDevice += '/';

            QByteArray mrl;
            switch (source.discType()) {
            case Phonon::NoDisc:
                kFatal(610) << "I should never get to see a MediaSource that is a disc but doesn't specify which one";
                return;
            case Phonon::Cd:
                mrl = autoplayMrlsToTitles("CD", "cdda:/");
                break;
            case Phonon::Dvd:
                mrl = "dvd:" + m_mediaDevice;
                break;
            case Phonon::Vcd:
                mrl = autoplayMrlsToTitles("VCD", "vcd:/");
                break;
            default:
                kError(610) << "media " << source.discType() << " not implemented";
                return;
            }
            switch (how) {
            case GaplessSwitch:
                m_stream->gaplessSwitchTo(mrl);
                break;
            case HardSwitch:
                m_shouldFakeBufferingOnPlay = true;
                m_stream->setMrl(mrl);
                break;
            }
        }
        break;
    case MediaSource::Stream:
        {
            // m_bytestream may not be deleted, the xine input plugin takes ownership and will
            // delete it when xine frees the input plugin
            m_bytestream = new ByteStream(source, this);
            switch (how) {
            case GaplessSwitch:
                m_stream->gaplessSwitchTo(m_bytestream->mrl());
                break;
            case HardSwitch:
                m_shouldFakeBufferingOnPlay = true;
                m_stream->setMrl(m_bytestream->mrl());
                break;
            }
        }
        break;
    }
    emit currentSourceChanged(m_mediaSource);
//X     if (state() != Phonon::LoadingState) {
//X         stop();
//X     }
}

//X void MediaObject::openMedia(Phonon::MediaObject::Media m, const QString &mediaDevice)
//X {
//X     m_titles.clear();
//X 
//X }

QByteArray MediaObject::autoplayMrlsToTitles(const char *plugin, const char *defaultMrl)
{
    const int lastSize = m_titles.size();
    m_titles.clear();
    int num = 0;
    char **mrls = xine_get_autoplay_mrls(m_stream->xine(), plugin, &num);
    for (int i = 0; i < num; ++i) {
        if (mrls[i]) {
            kDebug(610) << mrls[i];
            m_titles << QByteArray(mrls[i]);
        }
    }
    if (lastSize != m_titles.size()) {
        emit availableTitlesChanged(m_titles.size());
    }
    if (m_titles.isEmpty()) {
        return defaultMrl;
    }
    m_currentTitle = 1;
    if (m_autoplayTitles) {
        m_stream->useGaplessPlayback(true);
    } else {
        m_stream->useGaplessPlayback(false);
    }
    return m_titles.first();
}

bool MediaObject::hasInterface(Interface interface) const
{
    switch (interface) {
    case AddonInterface::TitleInterface:
        if (m_titles.size() > 1) {
            return true;
        }
        break;
    case AddonInterface::ChapterInterface:
        if (m_stream->availableChapters() > 1) {
            return true;
        }
        break;
    case AddonInterface::SubtitleInterface:
        if (m_stream->subtitlesSize() > 0) { //subtitles off by default, enable if any
            return true;
        }
        break;
    case AddonInterface::AudioChannelInterface:
        if (m_stream->audioChannelsSize() > 1) { //first audio channel on by default, enable if > 1
            return true;
        }
        break;
    }
    return false;
}

void MediaObject::handleAvailableTitlesChanged(int t)
{
    kDebug(610) << t;
    if (m_mediaSource.discType() == Phonon::Dvd) {
        QByteArray mrl = "dvd:" + m_mediaDevice;
        const int lastSize = m_titles.size();
        m_titles.clear();
        for (int i = 1; i <= t; ++i) {
            m_titles << mrl + QByteArray::number(i);
        }
        if (m_titles.size() != lastSize) {
            emit availableTitlesChanged(m_titles.size());
        }
    }
}

QVariant MediaObject::interfaceCall(Interface interface, int command, const QList<QVariant> &arguments)
{
    kDebug(610) << interface << ", " << command;

    switch (interface) {
    case AddonInterface::ChapterInterface:
        switch (static_cast<AddonInterface::ChapterCommand>(command)) {
        case AddonInterface::availableChapters:
            return m_stream->availableChapters();
        case AddonInterface::chapter:
            return m_stream->currentChapter();
        case AddonInterface::setChapter:
            {
                if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
                    kDebug(610) << "arguments invalid";
                    return false;
                }
                int c = arguments.first().toInt();
                int t = m_currentTitle - 1;
                if (t < 0) {
                    t = 0;
                }
                if (m_titles.size() > t) {
                    QByteArray mrl = m_titles[t] + '.' + QByteArray::number(c);
                    m_stream->setMrl(mrl, XineStream::KeepState);
                }
                return true;
            }
        }
        break;
    case AddonInterface::TitleInterface:
        switch (static_cast<AddonInterface::TitleCommand>(command)) {
        case AddonInterface::availableTitles:
            kDebug(610) << m_titles.size();
            return m_titles.size();
        case AddonInterface::title:
            kDebug(610) << m_currentTitle;
            return m_currentTitle;
        case AddonInterface::setTitle:
            {
                if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
                    kDebug(610) << "arguments invalid";
                    return false;
                }
                int t = arguments.first().toInt();
                if (t > m_titles.size()) {
                    kDebug(610) << "invalid title";
                    return false;
                }
                if (m_currentTitle == t) {
                    kDebug(610) << "no title change";
                    return true;
                }
                kDebug(610) << "change title from " << m_currentTitle << " to " << t;
                m_currentTitle = t;
                m_stream->setMrl(m_titles[t - 1],
                        m_autoplayTitles ? XineStream::KeepState : XineStream::StoppedState);
                if (m_mediaSource.discType() == Phonon::Cd) {
                    emit titleChanged(m_currentTitle);
                }
                return true;
            }
        case AddonInterface::autoplayTitles:
            return m_autoplayTitles;
        case AddonInterface::setAutoplayTitles:
            {
                if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Bool)) {
                    kDebug(610) << "arguments invalid";
                    return false;
                }
                bool b = arguments.first().toBool();
                if (b == m_autoplayTitles) {
                    kDebug(610) << "setAutoplayTitles: no change";
                    return false;
                }
                m_autoplayTitles = b;
                if (b) {
                    kDebug(610) << "setAutoplayTitles: enable autoplay";
                    m_stream->useGaplessPlayback(true);
                } else {
                    kDebug(610) << "setAutoplayTitles: disable autoplay";
                    m_stream->useGaplessPlayback(false);
                }
                return true;
            }
        }
        break;
    case AddonInterface::SubtitleInterface:
        switch (static_cast<AddonInterface::SubtitleCommand>(command))
        {
            case AddonInterface::availableSubtitles:
                return QVariant::fromValue( m_stream->availableSubtitles() );
            case AddonInterface::currentSubtitle:
                return QVariant::fromValue(m_stream->currentSubtitle());
            case AddonInterface::setCurrentSubtitle:
                if (arguments.isEmpty() || !arguments.first().canConvert<SubtitleDescription>() ) {
                    kDebug(610) << "arguments invalid";
                    return false;
                }
                m_stream->setCurrentSubtitle( arguments.first().value<SubtitleDescription>() );
                return true;
        }
        break;
    case AddonInterface::AudioChannelInterface:
        switch (static_cast<AddonInterface::AudioChannelCommand>(command))
        {
            case AddonInterface::availableAudioChannels:
                return QVariant::fromValue( m_stream->availableAudioChannels() );
            case AddonInterface::currentAudioChannel:
                return QVariant::fromValue( m_stream->currentAudioChannel() );
            case AddonInterface::setCurrentAudioChannel:
                if (arguments.isEmpty() || !arguments.first().canConvert<AudioChannelDescription>() ) {
                    kDebug(610) << "arguments invalid";
                    return false;
                }
                m_stream->setCurrentAudioChannel( arguments.first().value<AudioChannelDescription>() );
                return true;
         }
         break;
    }
    return QVariant();
}

void MediaObject::needNextUrl()
{
    if (m_mediaSource.type() == MediaSource::Disc && m_titles.size() > m_currentTitle) {
        m_stream->gaplessSwitchTo(m_titles[m_currentTitle]);
        ++m_currentTitle;
        emit titleChanged(m_currentTitle);
        return;
    }
    emit aboutToFinish();
}

void MediaObject::setPrefinishMark(qint32 newPrefinishMark)
{
    m_prefinishMark = newPrefinishMark;
    m_stream->setPrefinishMark(newPrefinishMark);
}

void MediaObject::handleHasVideoChanged(bool hasVideo)
{
    downstreamEvent(new HasVideoEvent(hasVideo));
}

void MediaObject::upstreamEvent(Event *e)
{
    Q_ASSERT(e);
    switch (e->type()) {
        case Event::IsThereAXineEngineForMe:
        // yes there is
        downstreamEvent(new HeresYourXineStreamEvent(stream()));
        break;
    case Event::UpdateVolume:
        kDebug() << "UpdateVolumeEvent";
        // postEvent takes ownership of the event and will delete it when done
        QCoreApplication::postEvent(m_stream, copyEvent(static_cast<UpdateVolumeEvent *>(e)));
        break;
    case Event::RequestSnapshot:
        // postEvent takes ownership of the event and will delete it when done
        QCoreApplication::postEvent(m_stream, new Event(e->type()));
        break;
    case Event::SetParam:
        // postEvent takes ownership of the event and will delete it when done
        QCoreApplication::postEvent(m_stream, copyEvent(static_cast<SetParamEvent *>(e)));
        break;
    case Event::EventSend:
        //kDebug(610) << "copying EventSendEvent and post it to XineStream" << m_stream;
        // postEvent takes ownership of the event and will delete it when done
        QCoreApplication::postEvent(m_stream, copyEvent(static_cast<EventSendEvent *>(e)));
        break;
    default:
        break;
    }
    SourceNode::upstreamEvent(e);
}

// the point of this reimplementation is to make downstreamEvent available as a slot
void MediaObject::downstreamEvent(Event *e)
{
    SourceNode::downstreamEvent(e);
}

}}

#include "mediaobject.moc"
// vim: sw=4 ts=4
