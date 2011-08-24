/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

namespace Phonon {
namespace Fake {

MediaObject::MediaObject(QObject *parent) :
    QObject(parent),
    m_source(),
    m_state(Phonon::LoadingState)
{
}

MediaObject::~MediaObject() {}

MediaSource MediaObject::source() const
{
    return m_source;
}

void MediaObject::setSource(const MediaSource &source)
{
    m_source = source;
}

void MediaObject::setNextSource(const MediaSource &source)
{
#warning nothing done with it
    m_nextSource = source;
}

void MediaObject::play()
{
    switch (m_state) {
    case StoppedState:
    case PausedState:
    case BufferingState:
    case LoadingState:
        m_pipeline.start();
        changeState(PlayingState);
        break;
    case ErrorState:
    case PlayingState:
        return;
    }
}

void MediaObject::pause()
{
    switch (m_state) {
    case StoppedState:
    case PlayingState:
    case BufferingState:
        m_pipeline.pause();
        changeState(PausedState);
        break;
    case ErrorState:
    case PausedState:
    case LoadingState:
        return;
    }
}

void MediaObject::stop()
{
#warning needs validation
    m_pipeline.stop();
    changeState(StoppedState);
}

bool MediaObject::isSeekable() const
{
#warning
    return true;
}

void MediaObject::seek(qint64 milliseconds)
{
#warning state dependent?
    m_pipeline.seek(milliseconds);
}

void MediaObject::changeState(Phonon::State newState)
{
    const State oldState = m_state;
    m_state = newState;
    emit stateChanged(newState, oldState);
}

qint32 MediaObject::tickInterval() const
{
#warning interval only can use int!!!!
    return m_pipeline.interval();
}

void MediaObject::setTickInterval(qint32 interval)
{
#warning interval only can use int!!!!
    m_pipeline.setInterval(interval);
}

bool MediaObject::hasVideo() const
{
#warning
    return true;
}

qint64 MediaObject::currentTime() const
{
    return m_pipeline.time();
}

State MediaObject::state() const
{
    return m_state;
}

QString MediaObject::errorString() const
{
#warning
    return QString();
}

ErrorType MediaObject::errorType() const
{
#warning
    return NoError;
}

qint64 MediaObject::totalTime() const
{
#warning
    return -1;
}

qint32 MediaObject::prefinishMark() const
{
    Q_ASSERT(false);
}

void MediaObject::setPrefinishMark(qint32)
{
    Q_ASSERT(false);
}

qint32 MediaObject::transitionTime() const
{
    Q_ASSERT(false);
}

void MediaObject::setTransitionTime(qint32)
{
    Q_ASSERT(false);
}

} // namespace Fake
} // namespace Phonon
