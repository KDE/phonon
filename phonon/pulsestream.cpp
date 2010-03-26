/*  This file is part of the KDE project
    Copyright (C) 2010 Colin Guthrie <cguthrie@mandriva.org>

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

#ifdef HAVE_PULSEAUDIO
#include <pulse/pulseaudio.h>
#else
#define PA_INVALID_INDEX ((uint32_t)-1)
#endif

#include "pulsestream.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

PulseStream::PulseStream(QString streamUuid)
  : QObject()
  , mStreamUuid(streamUuid)
  , mIndex(PA_INVALID_INDEX)
{
}

PulseStream::~PulseStream()
{
}

QString PulseStream::uuid()
{
    return mStreamUuid;
}

uint32_t PulseStream::index()
{
    return mIndex;
}

void PulseStream::setIndex(uint32_t index)
{
    mIndex = index;
}

void PulseStream::setDevice(int device)
{
    emit usingDevice(device);
}


} // namespace Phonon

QT_END_NAMESPACE

#include "moc_pulsestream.cpp"

// vim: sw=4 ts=4
