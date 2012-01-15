/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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

#include "audioformat.h"
#include "phonondefs_p.h"

namespace Phonon
{
namespace Experimental
{

class AudioFormatPrivate
{
    P_DECLARE_PUBLIC(AudioFormat)
    protected:
        AudioFormat *q_ptr;
};

AudioFormat::AudioFormat(int sampleRate, int channelCount, Phonon::Experimental::BitRate bitRate, QSysInfo::Endian byteOrder)
{
    s.m_sampleRate = sampleRate;
    s.m_channelCount = channelCount;
    s.m_bitRate = bitRate;
    s.m_byteOrder = byteOrder;
}

AudioFormat::~AudioFormat()
{
}

int AudioFormat::sampleRate() const
{
    return s.m_sampleRate;
}

int AudioFormat::channelCount() const
{
    return s.m_channelCount;
}

Phonon::Experimental::BitRate AudioFormat::bitRate() const
{
    return s.m_bitRate;
}

QSysInfo::Endian AudioFormat::byteOrder() const
{
    return s.m_byteOrder;
}

AudioFormat::AudioFormat(const AudioFormat &f)
{
    s.m_sampleRate = f.sampleRate();
    s.m_channelCount = f.channelCount();
    s.m_bitRate = f.bitRate();
    s.m_byteOrder = f.byteOrder();
}

AudioFormat &AudioFormat::operator=(const AudioFormat &f)
{
    s.m_sampleRate = f.sampleRate();
    s.m_channelCount = f.channelCount();
    s.m_bitRate = f.bitRate();
    s.m_byteOrder = f.byteOrder();
    return *this;
}

bool AudioFormat::operator==(const AudioFormat &f) const
{
    return s.m_sampleRate == f.sampleRate() &&
        s.m_channelCount == f.channelCount() &&
        s.m_bitRate == f.bitRate() &&
        s.m_byteOrder == f.byteOrder();
}

bool AudioFormat::operator<(const AudioFormat &f) const
{
    return s.m_bitRate < f.bitRate() ||
        (s.m_bitRate == f.bitRate() && (s.m_sampleRate < f.sampleRate() ||
                                      (s.m_sampleRate == f.sampleRate() && (s.m_channelCount < f.channelCount() ||
                                                                          (s.m_channelCount == f.channelCount() && s.m_byteOrder != QSysInfo::ByteOrder && f.byteOrder() == QSysInfo::ByteOrder)))));
}

quint32 AudioFormat::key() const
{
    return (s.m_byteOrder == QSysInfo::ByteOrder ? 1 : 0) + // 1 bit  least significant
        (s.m_channelCount << 1) + // give it 8 bits
        (s.m_sampleRate) + // 192kHz ~ 18 bits (let the (unimportant) lower 9 bits overlap with channels + byteOrder)
        (s.m_bitRate << 18); //                                       most significant
}

} // namespace Experimental
} // namespace Phonon
