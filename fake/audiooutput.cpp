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

#include "audiooutput.h"

namespace Phonon {
namespace Fake {

AudioOutput::AudioOutput(QObject *parent) :
    QObject(parent)
{
}

AudioOutput::~AudioOutput()
{
}

qreal AudioOutput::volume() const
{
    return m_volume;
}

void AudioOutput::setVolume(qreal volume)
{
    m_volume = volume;
}

int AudioOutput::outputDevice() const
{
#warning
    return -1;
}

bool AudioOutput::setOutputDevice(int index)
{
#warning
    Q_ASSERT(false);
}

bool AudioOutput::setOutputDevice(const Phonon::AudioOutputDevice &description)
{
#warning
    Q_ASSERT(false);
}

QList<QPair<QByteArray, QString> > AudioOutput::deviceAccessListFor(const Phonon::AudioOutputDevice &) const
{
#warning
    Q_ASSERT(false);
}

} // namespace Fake
} // namespace Phonon
