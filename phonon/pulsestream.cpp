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

#include "pulsestream_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

PulseStream::PulseStream(QString streamUuid)
  : QObject()
  , mStreamUuid(streamUuid)
  , mIndex(PA_INVALID_INDEX)
  , mDevice(-1)
  , mMute(false)
{
    pa_cvolume_init(&mVolume);
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

uint8_t PulseStream::channels()
{
    return mVolume.channels;
}

void PulseStream::setDevice(int device)
{
    if (mDevice != device) {
        mDevice = device;
        emit usingDevice(device);
    }
}

// Copied from AudioOutput
static const qreal LOUDNESS_TO_VOLTAGE_EXPONENT = qreal(0.67);
static const qreal VOLTAGE_TO_LOUDNESS_EXPONENT = qreal(1.0/LOUDNESS_TO_VOLTAGE_EXPONENT);

void PulseStream::setVolume(const pa_cvolume *volume)
{
    if (pa_cvolume_equal(&mVolume, volume) == 0) {
        memcpy(&mVolume, volume, sizeof(mVolume));
        qreal vol = (qreal)pa_cvolume_avg(volume) / PA_VOLUME_NORM;
        // AudioOutput expects the "backend" to supply values that have been
        // adjusted for Stephens' law, so we need to fudge them accordingly
        // so that the %ages match up in KMix/the application's own slider.
        emit volumeChanged(pow(vol, VOLTAGE_TO_LOUDNESS_EXPONENT));
    }
}

void PulseStream::setMute(bool mute)
{
    if (mMute != mute) {
        mMute = mute;
        emit muteChanged(mMute);
    }
}


} // namespace Phonon

QT_END_NAMESPACE

#include "moc_pulsestream_p.cpp"

// vim: sw=4 ts=4
