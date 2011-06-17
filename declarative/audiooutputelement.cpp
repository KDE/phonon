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

#include "audiooutputelement.h"

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>

#include "mediaelement.h"

namespace Phonon {
namespace Declarative {

#define SECURE if(!m_mediaObject && !m_audioOutput) init()

AudioOutputElement::AudioOutputElement(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_audioOutput(0)
{
}

AudioOutputElement::~AudioOutputElement()
{
}

void AudioOutputElement::init()
{
    if (m_audioOutput)
        return;

    MediaElement *mediaElement = qobject_cast<MediaElement *>(parentItem());
    Q_ASSERT(mediaElement);

    m_mediaObject = mediaElement->mediaObject();
    Q_ASSERT(m_mediaObject);

#warning todo: category
    m_audioOutput = new AudioOutput(this);

    createPath(m_mediaObject, m_audioOutput);
}

} // namespace Declarative
} // namespace Phonon
