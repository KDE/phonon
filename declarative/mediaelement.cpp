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

#include "mediaelement.h"

#include <phonon/mediaobject.h>

namespace Phonon {
namespace Declarative {

MediaElement::MediaElement(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_mediaObject(0),
    m_state(StoppedState),
    m_finished(false)
{
}

MediaElement::~MediaElement()
{
}

QUrl MediaElement::source() const
{
    return m_source;
}

void MediaElement::setSource(const QUrl &url)
{
    m_source = url;
    m_finished = false;
    if (m_mediaObject)
        m_mediaObject->setCurrentSource(MediaSource(url));
    emit sourceChanged();
}

bool MediaElement::isPlaying() const
{
    return m_state & PlayingState;
}

bool MediaElement::isStopped() const
{
    return m_state & StoppedState;
}

void MediaElement::play()
{
    init();
    m_finished = false;
    m_mediaObject->play();
}

void MediaElement::stop()
{
    init();
    m_finished = false;
    m_mediaObject->stop();
}

void MediaElement::handleFinished()
{
    m_finished = true;
}

void MediaElement::emitStateChanges(Phonon::State state)
{
    switch(state) {
    case StoppedState:
        emit stoppedChanged();
        break;
    case PlayingState:
        emit playingChanged();
        break;
    }
}

void MediaElement::handleStateChange(Phonon::State newState, Phonon::State oldState)
{
    if (newState == oldState && newState == m_state)
        return; // Simply reject this case altogether - darn you backends!!!

    m_state = newState;
    emitStateChanges(oldState);
    emitStateChanges(newState);

    // The defined behaviour is that the backend is in paused after playing is
    // finished. Completely pointless and I'd rather not have people care about it.
#warning this is ubergross as there is plenty of other states we could switch to ... when will me statemachine appear :(
    if (m_finished && oldState == PlayingState)
        m_mediaObject->stop();
}

void MediaElement::init()
{
    if (m_mediaObject)
        return;

    m_mediaObject = new MediaObject(this);
    m_mediaObject->setCurrentSource(MediaSource(m_source));

    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(handleStateChange(Phonon::State, Phonon::State)));
    connect(m_mediaObject, SIGNAL(finished()),
            this, SLOT(handleFinished()));
}


} // namespace Declarative
} // namespace Phonon
