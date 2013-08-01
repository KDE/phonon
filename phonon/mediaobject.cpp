/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011 Trever Fischer <tdfischer@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "mediaobject.h"
#include "mediaobject_p.h"

#include "factory_p.h"
#include "mediaobjectinterface.h"
#include "audiooutput.h"
#include "phonondefs_p.h"
#include "abstractmediastream.h"
#include "abstractmediastream_p.h"
#include "frontendinterface_p.h"

#include <QtCore/QStringBuilder>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#ifdef HAVE_QZEITGEIST
#include <QZeitgeist/DataModel/Event>
#include <QZeitgeist/Interpretation>
#include <QZeitgeist/Log>
#include <QZeitgeist/Manifestation>
#include <QZeitgeist/QZeitgeist>
#endif

#include "phononnamespace_p.h"
#include "platform_p.h"
#include "statesvalidator_p.h"

#ifndef PHONON_NO_GRAPHICSVIEW
#include <graphicsview/videographicsobject.h>
#endif

#define PHONON_CLASSNAME MediaObject
#define PHONON_INTERFACENAME MediaObjectInterface

namespace Phonon
{
PHONON_OBJECT_IMPL

MediaObject::~MediaObject()
{
    P_D(MediaObject);
    if (d->m_backendObject) {
        switch (state()) {
        case PlayingState:
        case BufferingState:
        case PausedState:
            stop();
            break;
        case ErrorState:
        case StoppedState:
        case LoadingState:
            break;
        }
    }
}

Phonon::State MediaObject::state() const
{
    P_D(const MediaObject);
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (d->errorOverride) {
        return d->state;
    }
    if (d->ignoreLoadingToBufferingStateChange) {
        return BufferingState;
    }
    if (d->ignoreErrorToLoadingStateChange) {
        return LoadingState;
    }
#endif // QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (!d->m_backendObject) {
        return d->state;
    }
    return INTERFACE_CALL(state());
}

PHONON_INTERFACE_SETTER(setTickInterval, tickInterval, qint32)
PHONON_INTERFACE_GETTER(qint32, tickInterval, d->tickInterval)
PHONON_INTERFACE_GETTER(bool, hasVideo, false)
PHONON_INTERFACE_GETTER(bool, isSeekable, false)
PHONON_INTERFACE_GETTER(qint64, currentTime, d->currentTime)

static inline bool isPlayable(const MediaSource::Type t)
{
    return t != MediaSource::Invalid && t != MediaSource::Empty;
}

void MediaObject::play()
{
    P_D(MediaObject);
    if (d->backendObject() && isPlayable(d->mediaSource.type())) {

    #ifndef PHONON_NO_GRAPHICSVIEW
        VideoGraphicsObject *vgo = 0;
        foreach (const Path &path, d->outputPaths) {
            if (vgo = dynamic_cast<VideoGraphicsObject *>(path.sink())) {
                // Play() is delayed until we had a paint event.
                if (!vgo->canNegotiate()) {
                    pWarning() << "VideoGraphicsObject not ready to be played because it has not received a paint() call yet, delaying playback.";
                    connect(vgo, SIGNAL(gotPaint()), this, SLOT(play()));
                    return;
                }
            }
        }
    #endif

        INTERFACE_CALL(play());
    }
}

void MediaObject::pause()
{
    P_D(MediaObject);
    if (d->backendObject() && isPlayable(d->mediaSource.type())) {
        INTERFACE_CALL(pause());
    }
}

void MediaObject::stop()
{
    P_D(MediaObject);
    if (d->backendObject() && isPlayable(d->mediaSource.type())) {
        INTERFACE_CALL(stop());
    }
}

void MediaObject::seek(qint64 time)
{
    P_D(MediaObject);
    if (d->backendObject() && isPlayable(d->mediaSource.type())) {
        INTERFACE_CALL(seek(time));
    }
}

QString MediaObject::errorString() const
{
    if (state() == Phonon::ErrorState) {
        P_D(const MediaObject);
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        if (d->errorOverride) {
            return d->errorString;
        }
#endif // QT_NO_PHONON_ABSTRACTMEDIASTREAM
        return INTERFACE_CALL(errorString());
    }
    return QString();
}

ErrorType MediaObject::errorType() const
{
    if (state() == Phonon::ErrorState) {
        P_D(const MediaObject);
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        if (d->errorOverride) {
            return d->errorType;
        }
#endif // QT_NO_PHONON_ABSTRACTMEDIASTREAM
        return INTERFACE_CALL(errorType());
    }
    return Phonon::NoError;
}

QStringList MediaObject::metaData(Phonon::MetaData f) const
{
    switch (f) {
    case ArtistMetaData:
        return metaData(QLatin1String("ARTIST"));
    case AlbumMetaData:
        return metaData(QLatin1String("ALBUM"));
    case TitleMetaData:
        return metaData(QLatin1String("TITLE"));
    case DateMetaData:
        return metaData(QLatin1String("DATE"));
    case GenreMetaData:
        return metaData(QLatin1String("GENRE"));
    case TracknumberMetaData:
        return metaData(QLatin1String("TRACKNUMBER"));
    case DescriptionMetaData:
        return metaData(QLatin1String("DESCRIPTION"));
    case MusicBrainzDiscIdMetaData:
        return metaData(QLatin1String("MUSICBRAINZ_DISCID"));
    }
    return QStringList();
}

QStringList MediaObject::metaData(const QString &key) const
{
    P_D(const MediaObject);
    return d->metaData.values(key);
}

QMultiMap<QString, QString> MediaObject::metaData() const
{
    P_D(const MediaObject);
    return d->metaData;
}

PHONON_INTERFACE_GETTER(qint32, prefinishMark, d->prefinishMark)
PHONON_INTERFACE_SETTER(setPrefinishMark, prefinishMark, qint32)

PHONON_INTERFACE_GETTER(qint32, transitionTime, d->transitionTime)
PHONON_INTERFACE_SETTER(setTransitionTime, transitionTime, qint32)

qint64 MediaObject::totalTime() const
{
    P_D(const MediaObject);
    if (!d->m_backendObject) {
        return -1;
    }
    return INTERFACE_CALL(totalTime());
}

qint64 MediaObject::remainingTime() const
{
    P_D(const MediaObject);
    if (!d->m_backendObject) {
        return -1;
    }
    qint64 ret = INTERFACE_CALL(remainingTime());
    if (ret < 0) {
        return -1;
    }
    return ret;
}

MediaSource MediaObject::currentSource() const
{
    P_D(const MediaObject);
    return d->mediaSource;
}

void MediaObject::setCurrentSource(const MediaSource &newSource)
{
    P_D(MediaObject);
    if (!k_ptr->backendObject()) {
        d->mediaSource = newSource;
        return;
    }

    pDebug() << Q_FUNC_INFO << newSource.type() << newSource.url() << newSource.deviceName();

    stop(); // first call stop as that often is the expected state
            // for setting a new URL

    d->mediaSource = newSource;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    d->abstractStream = 0; // abstractStream auto-deletes
    if (d->mediaSource.type() == MediaSource::Stream) {
        Q_ASSERT(d->mediaSource.stream());
        d->mediaSource.stream()->d_func()->setMediaObjectPrivate(d);
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

    d->playingQueuedSource = false;

    d->sendToZeitgeist(StoppedState);
    INTERFACE_CALL(setSource(d->mediaSource));
    d->sendToZeitgeist();
}

void MediaObject::clear()
{
    P_D(MediaObject);
    d->sourceQueue.clear();
    setCurrentSource(MediaSource());
}

QList<MediaSource> MediaObject::queue() const
{
    P_D(const MediaObject);
    return d->sourceQueue;
}

void MediaObject::setQueue(const QList<MediaSource> &sources)
{
    P_D(MediaObject);
    d->sourceQueue.clear();
    enqueue(sources);
}

void MediaObject::setQueue(const QList<QUrl> &urls)
{
    P_D(MediaObject);
    d->sourceQueue.clear();
    enqueue(urls);
}

void MediaObject::enqueue(const MediaSource &source)
{
    P_D(MediaObject);
    if (!isPlayable(d->mediaSource.type())) {
        // the current source is nothing valid so this source needs to become the current one
        setCurrentSource(source);
    } else {
        d->sourceQueue << source;
    }
}

void MediaObject::enqueue(const QList<MediaSource> &sources)
{
    for (int i = 0; i < sources.count(); ++i) {
        enqueue(sources.at(i));
    }
}

void MediaObject::enqueue(const QList<QUrl> &urls)
{
    for (int i = 0; i < urls.count(); ++i) {
        enqueue(urls.at(i));
    }
}

void MediaObject::clearQueue()
{
    P_D(MediaObject);
    d->sourceQueue.clear();
}

void MediaObjectPrivate::sendToZeitgeist(const QString &event_interpretation,
                                         const QString &event_manifestation,
                                         const QString &event_actor,
                                         const QDateTime &subject_timestamp,
                                         const QUrl &subject_uri,
                                         const QString &subject_text,
                                         const QString &subject_interpretation,
                                         const QString &subject_manifestation,
                                         const QString &subject_mimetype)
{
#ifdef HAVE_QZEITGEIST
    QZeitgeist::DataModel::Subject subject;
    QString url = subject_uri.toString();
    QString path = url.left(url.lastIndexOf(QLatin1Char('/')));
    subject.setUri(url);
    subject.setText(subject_text);
    subject.setInterpretation(subject_interpretation);
    subject.setManifestation(subject_manifestation);
    subject.setOrigin(path);
    subject.setMimeType(subject_mimetype);

    QZeitgeist::DataModel::SubjectList subjects;
    subjects << subject;

    QZeitgeist::DataModel::Event event;
    event.setTimestamp(subject_timestamp);
    event.setInterpretation(event_interpretation);
    event.setManifestation(event_manifestation);
    event.setActor(event_actor);
    event.setSubjects(subjects);

    QZeitgeist::DataModel::EventList events;
    events << event;

    QDBusPendingReply<QZeitgeist::DataModel::EventIdList> reply =
        log->insertEvents(events);
#else
    Q_UNUSED(event_interpretation)
    Q_UNUSED(event_manifestation)
    Q_UNUSED(event_actor)
    Q_UNUSED(subject_timestamp)
    Q_UNUSED(subject_uri)
    Q_UNUSED(subject_text)
    Q_UNUSED(subject_interpretation)
    Q_UNUSED(subject_manifestation)
    Q_UNUSED(subject_mimetype)
#endif
}

void MediaObjectPrivate::sendToZeitgeist(State eventState)
{
#ifdef HAVE_QZEITGEIST
    P_Q(MediaObject);
    if (readyForZeitgeist && q->property("PlaybackTracking").toBool()) {
        pDebug() << "Current state:" << eventState;
        QString eventInterpretation;
        switch (eventState) {
        case PlayingState:
            eventInterpretation = QZeitgeist::Interpretation::Event::ZGAccessEvent;
            break;
        case ErrorState:
        case StoppedState:
            eventInterpretation = QZeitgeist::Interpretation::Event::ZGLeaveEvent;
            break;
        //These states are not signifigant events.
        case LoadingState:
        case BufferingState:
        case PausedState:
            return;
            break;
        }

        QStringList titles = q->metaData(TitleMetaData);
        QStringList artists = q->metaData(ArtistMetaData);
        QString title;
        if (titles.empty()) {
            QString file = mediaSource.url().toString();
            title = file.right(file.length()-file.lastIndexOf("/")-1);
        } else {
            if (artists.empty()) {
                title = titles[0];
            } else {
                title = QString(QObject::tr("%0 by %1")).arg(titles[0]).arg(artists[0]);
            }
        }
        pDebug() << "Sending" << title << "to zeitgeist";

        QString mime;
        QString subjectInterpretation;
        if (q->hasVideo()) {
            subjectInterpretation = QZeitgeist::Interpretation::Subject::NFOVideo;
            mime = "video/raw";
        } else {
            subjectInterpretation = QZeitgeist::Interpretation::Subject::NFOAudio;
            mime = "audio/raw";
        }
        pDebug() << "Zeitgeist mime type:" << mime;
        pDebug() << "Zeitgeist URL:" << mediaSource.url();
        pDebug() << "mediasource type:" << mediaSource.type();

        QString subjectType;
        switch (mediaSource.type()) {
        case MediaSource::Empty:
        case MediaSource::Invalid:
            return;
        case MediaSource::Url:
            subjectType = QZeitgeist::Manifestation::Subject::NFORemoteDataObject;
            break;
        case MediaSource::CaptureDevice:
        case MediaSource::Disc:
        case MediaSource::Stream:
            subjectType = QZeitgeist::Manifestation::Subject::NFOMediaStream;
            break;
        case MediaSource::LocalFile:
            subjectType = QZeitgeist::Manifestation::Subject::NFOFileDataObject;
            break;
        }

        QString eventManifestation;
        if (playingQueuedSource)
            eventManifestation = QZeitgeist::Manifestation::Event::ZGScheduledActivity;
        else
            eventManifestation = QZeitgeist::Manifestation::Event::ZGUserActivity;

        sendToZeitgeist(eventInterpretation,
                        eventManifestation,
                        QLatin1Literal("application://" ) % Platform::applicationName() % QLatin1Literal(".desktop"),
                        QDateTime::currentDateTime(),
                        mediaSource.url(),
                        title,
                        subjectInterpretation,
                        subjectType,
                        mime);
    }
    // Unset this so we don't send it again after a pause+play
    readyForZeitgeist = false;
    playingQueuedSource = false;
#else
    Q_UNUSED(eventState)
#endif
}

void MediaObjectPrivate::sendToZeitgeist()
{
    P_Q(MediaObject);
    sendToZeitgeist(q->state());
}

bool MediaObjectPrivate::aboutToDeleteBackendObject()
{
    //pDebug() << Q_FUNC_INFO;
    prefinishMark = pINTERFACE_CALL(prefinishMark());
    transitionTime = pINTERFACE_CALL(transitionTime());
    //pDebug() << Q_FUNC_INFO;
    if (m_backendObject) {
        state = pINTERFACE_CALL(state());
        currentTime = pINTERFACE_CALL(currentTime());
        tickInterval = pINTERFACE_CALL(tickInterval());
    }
    return true;
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
void MediaObjectPrivate::streamError(Phonon::ErrorType type, const QString &text)
{
    P_Q(MediaObject);
    State lastState = q->state();
    errorOverride = true;
    errorType = type;
    errorString = text;
    state = ErrorState;
    QMetaObject::invokeMethod(q, "stateChanged", Qt::QueuedConnection, Q_ARG(Phonon::State, Phonon::ErrorState), Q_ARG(Phonon::State, lastState));
    //emit q->stateChanged(ErrorState, lastState);
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

// TODO: this needs serious cleanup...
void MediaObjectPrivate::_k_stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    P_Q(MediaObject);

    // Zeitgeist ---------------------------------------------------------------
    if (newstate == StoppedState) {
        readyForZeitgeist = true;
    }
    pDebug() << "State changed from" << oldstate << "to" << newstate << "-> sending to zeitgeist.";
    sendToZeitgeist(newstate);

    // AbstractMediaStream fallback stuff --------------------------------------
    if (errorOverride) {
        errorOverride = false;
        if (newstate == ErrorState) {
            return;
        }
        oldstate = ErrorState;
    }

    if (mediaSource.type() != MediaSource::Url) {
        // special handling only necessary for URLs because of the fallback
        emit q->stateChanged(newstate, oldstate);
        return;
    }

    // backend MediaObject reached ErrorState, try a KioMediaSource
    if (newstate == Phonon::ErrorState && !abstractStream) {
        abstractStream = Platform::createMediaStream(mediaSource.url(), q);
        if (!abstractStream) {
            pDebug() << "backend MediaObject reached ErrorState, no KIO fallback available";
            emit q->stateChanged(newstate, oldstate);
            return;
        }
        pDebug() << "backend MediaObject reached ErrorState, trying Platform::createMediaStream now";
        ignoreLoadingToBufferingStateChange = false;
        ignoreErrorToLoadingStateChange = false;
        switch (oldstate) {
        case Phonon::BufferingState:
            // play() has already been called, we need to make sure it is called
            // on the backend with the KioMediaStream MediaSource now, too
            ignoreLoadingToBufferingStateChange = true;
            break;
        case Phonon::LoadingState:
            ignoreErrorToLoadingStateChange = true;
            // no extras
            break;
        default:
            pError() << "backend MediaObject reached ErrorState after " << oldstate
                << ". It seems a KioMediaStream will not help here, trying anyway.";
            emit q->stateChanged(Phonon::LoadingState, oldstate);
            break;
        }
        abstractStream->d_func()->setMediaObjectPrivate(this);
        MediaSource mediaSource(abstractStream);
        mediaSource.setAutoDelete(true);
        sendToZeitgeist(StoppedState);
        pINTERFACE_CALL(setSource(mediaSource));
        sendToZeitgeist();
        if (oldstate == Phonon::BufferingState) {
            q->play();
        }
        return;
    } else if (ignoreLoadingToBufferingStateChange &&
            abstractStream &&
            oldstate == Phonon::LoadingState) {
        if (newstate != Phonon::BufferingState) {
            emit q->stateChanged(newstate, Phonon::BufferingState);
        }
        return;
    } else if (ignoreErrorToLoadingStateChange && abstractStream && oldstate == ErrorState) {
        if (newstate != LoadingState) {
            emit q->stateChanged(newstate, Phonon::LoadingState);
        }
        return;
    }

    emit q->stateChanged(newstate, oldstate);
}

void MediaObjectPrivate::_k_aboutToFinish()
{
    P_Q(MediaObject);
    pDebug() << Q_FUNC_INFO;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    abstractStream = 0; // abstractStream auto-deletes
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

    if (sourceQueue.isEmpty()) {
        emit q->aboutToFinish();
        if (sourceQueue.isEmpty()) {
            return;
        }
    }

    mediaSource = sourceQueue.head();
    readyForZeitgeist = false;
    playingQueuedSource = true;
    pINTERFACE_CALL(setNextSource(mediaSource));

    if (validator)
        validator->sourceQueued();
}

void MediaObjectPrivate::_k_currentSourceChanged(const MediaSource &source)
{
    P_Q(MediaObject);
    pDebug() << Q_FUNC_INFO;

    if (!sourceQueue.isEmpty() && sourceQueue.head() == source)
        sourceQueue.dequeue();

    emit q->currentSourceChanged(source);
}

void MediaObjectPrivate::setupBackendObject()
{
    P_Q(MediaObject);
    Q_ASSERT(m_backendObject);

    // Queue *everything* there is. That way the backend always is in a defined state.
    // If the signals were not queued, and the backend emitted something mid-execution
    // of whatever it is doing, an API consumer works with an undefined state.
    // This causes major headaches. If we must enforce implicit execution stop via
    // signals, they ought to be done in private slots.

    qRegisterMetaType<MediaSource>("MediaSource");
    qRegisterMetaType<QMultiMap<QString, QString> >("QMultiMap<QString, QString>");

    if (validateStates)
        validator = new StatesValidator(q); // Parented, and non-invasive to MO.

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
                     q, SLOT(_k_stateChanged(Phonon::State,Phonon::State)), Qt::QueuedConnection);
#else
    QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
                     q, SIGNAL(stateChanged(Phonon::State,Phonon::State)), Qt::QueuedConnection);
#endif // QT_NO_PHONON_ABSTRACTMEDIASTREAM
#ifndef QT_NO_PHONON_VIDEO
    QObject::connect(m_backendObject, SIGNAL(hasVideoChanged(bool)),
                     q, SIGNAL(hasVideoChanged(bool)), Qt::QueuedConnection);
#endif //QT_NO_PHONON_VIDEO

    QObject::connect(m_backendObject, SIGNAL(tick(qint64)),
                     q, SIGNAL(tick(qint64)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(seekableChanged(bool)),
                     q, SIGNAL(seekableChanged(bool)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(bufferStatus(int)),
                     q, SIGNAL(bufferStatus(int)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(finished()),
                     q, SIGNAL(finished()), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(aboutToFinish()),
                     q, SLOT(_k_aboutToFinish()), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(prefinishMarkReached(qint32)),
                     q, SIGNAL(prefinishMarkReached(qint32)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(totalTimeChanged(qint64)),
                     q, SIGNAL(totalTimeChanged(qint64)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(metaDataChanged(QMultiMap<QString,QString>)),
                     q, SLOT(_k_metaDataChanged(QMultiMap<QString,QString>)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentSourceChanged(MediaSource)),
                     q, SLOT(_k_currentSourceChanged(MediaSource)), Qt::QueuedConnection);

    // set up attributes
    pINTERFACE_CALL(setTickInterval(tickInterval));
    pINTERFACE_CALL(setPrefinishMark(prefinishMark));
    pINTERFACE_CALL(setTransitionTime(transitionTime));

    switch(state)
    {
    case LoadingState:
    case StoppedState:
    case ErrorState:
        break;
    case PlayingState:
    case BufferingState:
        QTimer::singleShot(0, q, SLOT(_k_resumePlay()));
        break;
    case PausedState:
        QTimer::singleShot(0, q, SLOT(_k_resumePause()));
        break;
    }
    const State backendState = pINTERFACE_CALL(state());
    if (state != backendState && state != ErrorState) {
        // careful: if state is ErrorState we might be switching from a
        // MediaObject to a ByteStream for KIO fallback. In that case the state
        // change to ErrorState was already suppressed.
        pDebug() << "emitting a state change because the backend object has been replaced";
        emit q->stateChanged(backendState, state);
        state = backendState;
    }

#ifndef QT_NO_PHONON_MEDIACONTROLLER
    for (int i = 0 ; i < interfaceList.count(); ++i) {
        interfaceList.at(i)->_backendObjectChanged();
    }
#endif //QT_NO_PHONON_MEDIACONTROLLER

    // set up attributes
    if (isPlayable(mediaSource.type())) {
        readyForZeitgeist = false;
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        if (mediaSource.type() == MediaSource::Stream) {
            Q_ASSERT(mediaSource.stream());
            mediaSource.stream()->d_func()->setMediaObjectPrivate(this);
        }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
        sendToZeitgeist(StoppedState);
        pINTERFACE_CALL(setSource(mediaSource));
        sendToZeitgeist();
    }
}

void MediaObjectPrivate::_k_resumePlay()
{
    qobject_cast<MediaObjectInterface *>(m_backendObject)->play();
    if (currentTime > 0) {
        qobject_cast<MediaObjectInterface *>(m_backendObject)->seek(currentTime);
    }
}

void MediaObjectPrivate::_k_resumePause()
{
    pINTERFACE_CALL(pause());
    if (currentTime > 0) {
        qobject_cast<MediaObjectInterface *>(m_backendObject)->seek(currentTime);
    }
}

void MediaObjectPrivate::_k_metaDataChanged(const QMultiMap<QString, QString> &newMetaData)
{
    metaData = newMetaData;
    emit q_func()->metaDataChanged();
    pDebug() << "Metadata ready, sending to zeitgeist";
    readyForZeitgeist = true;
    sendToZeitgeist();
}

void MediaObjectPrivate::phononObjectDestroyed(MediaNodePrivate *bp)
{
    // this method is called from Phonon::Base::~Base(), meaning the AudioPath
    // dtor has already been called, also virtual functions don't work anymore
    // (therefore qobject_cast can only downcast from Base)
    Q_ASSERT(bp);
    Q_UNUSED(bp);
}

MediaObject *createPlayer(Phonon::Category category, const MediaSource &source)
{
    MediaObject *mo = new MediaObject;
    AudioOutput *ao = new AudioOutput(category, mo);
    createPath(mo, ao);
    if (isPlayable(source.type())) {
        mo->setCurrentSource(source);
    }
    return mo;
}

} //namespace Phonon

#include "moc_mediaobject.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME
// vim: sw=4 tw=100 et
