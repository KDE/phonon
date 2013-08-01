/*
    Copyright (C) 2012 Trever Fischer <tdfischer@fedoraproject.org>
    Copyright (C) 2012 Harald Sitter <sitter@kde.org>

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

#include "statesvalidator_p.h"

#include "mediaobject.h"
#include "phononnamespace_p.h"

#ifdef PHONON_ASSERT_STATES
#define P_INVALID_STATE(msg) Q_ASSERT_X(0, __FILE__, msg)
#else
#define P_INVALID_STATE(msg) pDebug() << "State assert failed:" << msg
#endif

namespace Phonon
{

StatesValidator::StatesValidator(MediaObject *parent)
    : QObject(parent)
    , m_mediaObject(parent)
    , m_prevState(Phonon::ErrorState)
    , m_sourceQueued(false)
    , m_aboutToFinishEmitted(false)
    , m_aboutToFinishBeforeSeek(false)
    , m_aboutToFinishPos(-1)
{
    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(validateStateChange(Phonon::State,Phonon::State)));
    connect(m_mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(validateSourceChange()));
    connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(validateTick(qint64)));
    connect(m_mediaObject, SIGNAL(aboutToFinish()), this, SLOT(validateAboutToFinish()));
    connect(m_mediaObject, SIGNAL(finished()), this, SLOT(validateFinished()));
    connect(m_mediaObject, SIGNAL(bufferStatus(int)), this, SLOT(validateBufferStatus()));
}

StatesValidator::~StatesValidator()
{
}

/**
 * The aboutToFinish signal is emitted when the queue is coming to an end.
 * This in particular means that it must not be emitted twice, unless no track
 * is in the queue and the user seeked back in time before finished
 * Since we track the frontend signal here, we only get this signal when the
 * queue is in fact empty in the frontend.
 * It can however happen that the frontend already delivered the last queue item,
 * then the user seeks and the backend forgets to use the already delivered item,
 * emitting a bogus aboutToFinish.
 */
void StatesValidator::validateAboutToFinish()
{
    if (m_aboutToFinishEmitted)
        P_INVALID_STATE("aboutToFinish emitted more than once!");
    m_aboutToFinishEmitted = true;
    m_aboutToFinishPos = m_pos;
}

void StatesValidator::validateFinished()
{
    if (m_mediaObject->state() != Phonon::PlayingState)
        P_INVALID_STATE("Playback finished when we weren't playing!");
}

void StatesValidator::validateSourceChange()
{
    if (m_mediaObject->state() != Phonon::StoppedState
            && m_mediaObject->state() != Phonon::PlayingState
            && m_mediaObject->state() != Phonon::PausedState
            && m_mediaObject->state() != Phonon::BufferingState) {
        P_INVALID_STATE("Source got changed outside a valid state");
    }
    m_sourceQueued = false; // Once we get the signal the backend internal one-source queue is definitely cleared.
    m_aboutToFinishEmitted = false;
    m_aboutToFinishBeforeSeek = false;
}

void StatesValidator::validateBufferStatus()
{
    if (m_mediaObject->state() != Phonon::PlayingState
            && m_mediaObject->state() != Phonon::PausedState
            && m_mediaObject->state() != Phonon::BufferingState) {
        P_INVALID_STATE("Buffer status changed when we weren't supposed to be buffering");
    }
}

void StatesValidator::validateTick(qint64 pos)
{
    // Mind that Buffering is a concurrent state, you may have been playing and
    // then start buffering for example for a seek.
    if (m_mediaObject->state() != Phonon::PlayingState
            && (m_prevState != Phonon::PlayingState
                && m_mediaObject->state() != Phonon::BufferingState))
        P_INVALID_STATE("Received tick outside of Playing state.");
    // If and only if we did not queue a new source may a seek back in time
    // result in a reemission of the signal. It should not, but it is allowed.
    // Point being, if the API consumer did not set one the first time, they
    // likely will not care about it a second time either.
    if (m_aboutToFinishEmitted && (pos < m_aboutToFinishPos) && !m_sourceQueued)
        m_aboutToFinishEmitted = false;
    m_pos = pos;
}

void StatesValidator::validateStateChange(Phonon::State newstate, Phonon::State oldstate)
{
    if (!validateStateTransition(newstate, oldstate)) {
        pDebug() << "Invalid state transition:" << oldstate << "->" << newstate;
        P_INVALID_STATE("Invalid state transition");
    } else {
        pDebug() << "Valid state transition:" << oldstate << "->" << newstate;
    }
    m_prevState = oldstate;
}

bool StatesValidator::validateStateTransition(Phonon::State newstate, Phonon::State oldstate)
{
    // Non-playback states trigger a reset of aboutToFinish.
    switch (oldstate) {
    case Phonon::StoppedState:
        switch (newstate) {
        case Phonon::LoadingState:
        case Phonon::PlayingState:
        case Phonon::PausedState:
            return true;
        default:
            return false;
        }
        break;
    case Phonon::LoadingState:
        switch (newstate) {
        case Phonon::ErrorState:
        case Phonon::StoppedState:
            return true;
        default:
            return false;
        }
        break;
    case Phonon::ErrorState:
        switch (newstate) {
        case Phonon::LoadingState:
            return true;
        default:
            return false;
        }
        break;
    case Phonon::PlayingState:
        switch (newstate) {
        case Phonon::PausedState:
        case Phonon::BufferingState:
        case Phonon::ErrorState:
        case Phonon::StoppedState:
            return true;
        default:
            return false;
        }
        break;
    case Phonon::PausedState:
        switch (newstate) {
        case Phonon::PlayingState:
        case Phonon::BufferingState:
        case Phonon::ErrorState:
        case Phonon::StoppedState:
            return true;
        default:
            return false;
        }
        break;
    case Phonon::BufferingState:
        switch (newstate) {
        case Phonon::PlayingState:
        case Phonon::PausedState:
        case Phonon::ErrorState:
// TODO: buffering state needs fixing, should not transit to stop
        case Phonon::StoppedState:
            return true;
        default:
            return false;
        }
    }
    return false;
}

} // namespace Phonon

