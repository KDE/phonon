/*
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011 Trever Fischer <tdfischer@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#include "player.h"
#include "player_p.h"

#include "abstractoutput.h"
#include "abstractoutput_p.h"
#include "factory_p.h"

#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include "phononnamespace_p.h"

namespace Phonon {

Player::Player(QObject *parent)
    : QObject(parent)
    , Frontend(*new PlayerPrivate)
{
}

Player::~Player()
{
    P_D(Player);
    if (!d->m_backendObject)
        return;

    switch (state()) {
    case PlayingState:
    case PausedState:
        stop();
        break;
    case StoppedState:
        break;
    }
}

Phonon::State Player::state() const
{
    P_D(const Player);
    if (!d->m_backendObject) {
        return d->state;
    }
    return d->interface->state();
}

void Player::setTickInterval(qint32 newTickInterval)
{
    P_D(Player);
#warning caching is inconsistent and semi broken ...
    if (d->interface)
        d->interface->setTickInterval(newTickInterval);
    else
        d->tickInterval = newTickInterval;
}

qint32 Player::tickInterval() const
{
    P_D(const Player);
    if (!d->interface)
        return d->tickInterval;
    return d->interface->tickInterval();
}

bool Player::isSeekable() const
{
    P_D(const Player);
    if (!d->interface)
        return false;
    return d->interface->isSeekable();
}

qint64 Player::currentTime() const
{
    P_D(const Player);
    if (!d->interface)
        return d->currentTime;
    return d->interface->currentTime();
}

static inline bool isPlayable(const Source::Type t)
{
    return t != Source::Invalid && t != Source::Empty;
}

void Player::play()
{
    P_D(Player);
    if (d->interface && isPlayable(d->mediaSource.type()))
        d->interface->play();
}

void Player::pause()
{
    P_D(Player);
    if (d->interface && isPlayable(d->mediaSource.type()))
        d->interface->pause();
}

void Player::stop()
{
    P_D(Player);
    if (d->interface && isPlayable(d->mediaSource.type()))
        d->interface->stop();
}

void Player::seek(qint64 time)
{
    P_D(Player);
    if (d->interface && isPlayable(d->mediaSource.type()))
        d->interface->seek(time);
}

QStringList Player::metaData(MetaData f) const
{
    P_D(const Player);
    return d->metaData.values(f);
}

QMultiMap<MetaData, QString> Player::metaData() const
{
    P_D(const Player);
    return d->metaData;
}

qint64 Player::totalTime() const
{
    P_D(const Player);
    if (!d->interface)
        return -1;
    return d->interface->totalTime();
}

qint64 Player::remainingTime() const
{
    P_D(const Player);
    if (!d->interface)
        return -1;

    const qint64 ret = d->interface->remainingTime();
    if (ret < -1)
        return -1; // Less than -1 is not a valid return value from us.
    return ret;
}

#warning bool and drop qwarning
void Player::addOutput(AbstractOutput *output)
{
    P_D(Player);
    if (d->outputs.contains(output))
        return;
    d->outputs.append(output);
    d->createBackendObject();
    if (!d->interface)
        qWarning("No PlayerInterface instance present.");
    d->interface->addOutput(output->k_func()->backendObject());
}

Source Player::source() const
{
    P_D(const Player);
    return d->mediaSource;
}

void Player::setSource(const Source &newSource)
{
    P_D(Player);
    if (!d->interface) {
        d->mediaSource = newSource;
        return;
    }

    pDebug() << Q_FUNC_INFO << newSource.type() << newSource.url() << newSource.deviceName();

    // first call stop as that often is the expected state for setting a new URL
    stop();

    d->mediaSource = newSource;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    d->abstractStream = 0; // abstractStream auto-deletes
    if (d->mediaSource.type() == Source::Stream) {
        Q_ASSERT(d->mediaSource.stream());
        d->mediaSource.stream()->d_func()->setMediaObjectPrivate(d);
    }

    d->playingQueuedSource = false;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

    d->interface->setSource(d->mediaSource);
}

void PlayerPrivate::createBackendObject()
{
    if (m_backendObject)
        return;

    P_Q(Player);
    m_backendObject = Factory::createPlayer(q);
    interface = qobject_cast<PlayerInterface *>(m_backendObject);
    if (m_backendObject && interface) // We need to have at least the base interface.
        setupBackendObject();
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
void PlayerPrivate::streamError(Phonon::ErrorType type, const QString &text)
{
    P_Q(Player);
    State lastState = q->state();
    errorType = type;
    errorString = text;
    state = ErrorState;
    QMetaObject::invokeMethod(q, "stateChanged", Qt::QueuedConnection, Q_ARG(Phonon::State, Phonon::ErrorState), Q_ARG(Phonon::State, lastState));
    //emit q->stateChanged(ErrorState, lastState);
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

void PlayerPrivate::setupBackendObject()
{
    P_Q(Player);
    Q_ASSERT(m_backendObject);

    // Queue *everything* there is. That way the backend always is in a defined state.
    // If the signals were not queued, and the backend emitted something mid-execution
    // of whatever it is doing, an API consumer works with an undefined state.
    // This causes major headaches. If we must enforce implicit execution stop via
    // signals, they ought to be done in private slots.

    qRegisterMetaType<Source>("Source");
    qRegisterMetaType<QMultiMap<QString, QString> >("QMultiMap<QString, QString>");

    QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
                     q, SIGNAL(stateChanged(Phonon::State, Phonon::State)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(tick(qint64)),
                     q, SIGNAL(tick(qint64)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(seekableChanged(bool)),
                     q, SIGNAL(seekableChanged(bool)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(bufferStatus(int)),
                     q, SIGNAL(bufferStatus(int)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(totalTimeChanged(qint64)),
                     q, SIGNAL(totalTimeChanged(qint64)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(metaDataChanged(QMultiMap<MetaData, QString>)),
                     q, SLOT(_k_metaDataChanged(QMultiMap<MetaData, QString>)), Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentSourceChanged(Source)),
                     q, SIGNAL(currentSourceChanged(Source)), Qt::QueuedConnection);

    // set up attributes
    interface->setTickInterval(tickInterval);

    switch(state) {
    case StoppedState:
        break;
    case PlayingState:
        QTimer::singleShot(0, q, SLOT(_k_resumePlay()));
        break;
    case PausedState:
        QTimer::singleShot(0, q, SLOT(_k_resumePause()));
        break;
    }

    const State backendState = interface->state();
    if (state != backendState) {
#warning do we really want this? ... shouldnt we force the frontend state on the backend instead?
        pDebug() << "emitting a state change because the backend object has been created";
        emit q->stateChanged(backendState, state);
        state = backendState;
    }

    // set up attributes
    if (isPlayable(mediaSource.type())) {
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        if (mediaSource.type() == Source::Stream) {
            Q_ASSERT(mediaSource.stream());
            mediaSource.stream()->d_func()->setMediaObjectPrivate(this);
        }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
        interface->setSource(mediaSource);
    }
}

void PlayerPrivate::_k_resumePlay()
{
    if (!interface)
        return; // Cannot happen short of bad eventloop timing.

    interface->play();
    if (currentTime > 0)
        interface->seek(currentTime);
}

void PlayerPrivate::_k_resumePause()
{
    if (!interface)
        return; // Cannot happen short of bad eventloop timing.

    interface->pause();
    if (currentTime > 0)
        interface->seek(currentTime);
}

void PlayerPrivate::_k_metaDataChanged(const QMultiMap<MetaData, QString> &newMetaData)
{
    metaData = newMetaData;
    emit q_func()->metaDataChanged();
}

} //namespace Phonon

#include "moc_player.cpp"
