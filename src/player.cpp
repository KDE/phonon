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
#include "debug_p.h"
#include "factory_p.h"
#include "phononglobal.h"

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

qint64 Player::time() const
{
    P_D(const Player);
    if (!d->interface)
        return 0;
    return d->interface->time();
}

void Player::play()
{
    P_D(Player);
    if (!d->m_backendObject)
        d->createBackendObject();
    if (d->interface)
        d->interface->play();
}

void Player::pause()
{
    P_D(Player);
    if (d->interface)
        d->interface->pause();
}

void Player::stop()
{
#warning stop needs to force a state reset including signals (i.e. stop should not only stop but cause the gui to reflect that...)
    P_D(Player);
    if (d->interface)
        d->interface->stop();
}

void Player::seek(qint64 time)
{
    P_D(Player);
    if (d->interface)
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

bool Player::isPlaying() const
{
    P_D(const Player);
    return state() == PlayingState;
}

bool Player::isPaused() const
{
    P_D(const Player);
    return state() == PausedState;
}

bool Player::isStopped() const
{
    P_D(const Player);
    return state() == StoppedState;
}

bool Player::addOutput(QObject *output)
{
    P_D(Player);
    AbstractOutput *abstractOutput = dynamic_cast<AbstractOutput *>(output);
    d->createBackendObject();
    if (!d->interface || !abstractOutput) // Couldn't create object or output is rubbish.
        return false;
    if (d->outputs.contains(abstractOutput)) // Already linked.
        return false;
    const bool ret = d->interface->addOutput(abstractOutput->k_func()->backendObject());
    if (ret) // Only add to list iff linked by the backend.
        d->outputs.append(abstractOutput);
    return ret;
}

Source Player::source() const
{
    P_D(const Player);
    return d->source;
}

void Player::setSource(const Source &newSource)
{
    P_D(Player);
    if (!d->interface) {
        d->source = newSource;
        return;
    }

    pDebug() << Q_FUNC_INFO << newSource.url() << newSource.deviceName();

    // first call stop as that often is the expected state for setting a new URL
    stop();

    d->source = newSource;

    d->abstractStream = 0; // abstractStream auto-deletes

    d->interface->setSource(d->source);
}

QUrl Player::url() const
{
    return source().url();
}

void Player::setUrl(const QUrl &url)
{
    setSource(Source(url));
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

#warning we definitely need to cut down on queuing
    QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
                     q, SLOT(_p_stateChanged(Phonon::State, Phonon::State)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(timeChanged(qint64)),
                     q, SIGNAL(timeChanged(qint64)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(seekableChanged(bool)),
                     q, SIGNAL(seekableChanged(bool)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(bufferStatus(int)),
                     q, SIGNAL(bufferStatus(int)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(totalTimeChanged(qint64)),
                     q, SIGNAL(totalTimeChanged(qint64)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(metaDataChanged(QMultiMap<MetaData, QString>)),
                     q, SLOT(_p_metaDataChanged(QMultiMap<MetaData, QString>)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(sourceChanged(Source)),
                     q, SIGNAL(sourceChanged(Source)),
                     Qt::QueuedConnection);

    // set up attributes
    interface->setTickInterval(tickInterval);

    const State backendState = interface->state();
    if (state != backendState) {
#warning do we really want this? ... shouldnt we force the frontend state on the backend instead?
        pDebug() << "emitting a state change because the backend object has been created";
        emit q->stateChanged(backendState, state);
        state = backendState;
    }

    interface->setSource(source);
}

// MetaData in the backend may be cached, or it may not be cached.
// Since the map construction however almost always requires excessive construction
// and conversion of data types it generally makes sense to have arbitrary
// data queries go into an always present cache in libphonon rather than hoping
// that the backend implements a cache for it.
// Additionally this in theory allows discarding data changes when there is no
// difference, which ought to prevent excessive GUI redraws/updates.
void PlayerPrivate::_p_metaDataChanged(const QMultiMap<MetaData, QString> &newMetaData)
{
    metaData = newMetaData;
    emit q_func()->metaDataChanged();
}

void PlayerPrivate::_p_stateChanged(State newState, State oldState)
{
    P_Q(Player);

    if (newState == oldState)
        return;

    state = newState;

    switch (newState) {
    case PlayingState:
        emit q->playingChanged(true);
        break;
    case PausedState:
        emit q->pausedChanged(true);
        break;
    case StoppedState:
        emit q->stoppedChanged(true);
        break;
    }

    switch (oldState) {
    case PlayingState:
        emit q->playingChanged(false);
        break;
    case PausedState:
        emit q->pausedChanged(false);
        break;
    case StoppedState:
        emit q->stoppedChanged(false);
        break;
    }

    emit q->stateChanged(newState, oldState);
}

} //namespace Phonon

#include "moc_player.cpp"
