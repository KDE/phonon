/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "audiodataoutput.h"
#include <QtCore/QVector>
#include <QtCore/QMap>

namespace Phonon
{
namespace Fake
{
AudioDataOutput::AudioDataOutput(QObject *parent)
    : AbstractAudioOutput(parent)
{
}

AudioDataOutput::~AudioDataOutput()
{
}

Phonon::Experimental::AudioDataOutput::Format AudioDataOutput::format() const
{
    return m_format;
}

int AudioDataOutput::dataSize() const
{
    return m_dataSize;
}

int AudioDataOutput::sampleRate() const
{
    return 44100;
}

void AudioDataOutput::setFormat(Phonon::Experimental::AudioDataOutput::Format format)
{
    m_format = format;
}

void AudioDataOutput::setDataSize(int size)
{
    m_dataSize = size;
}

typedef QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<float> > FloatMap;
typedef QMap<Phonon::Experimental::AudioDataOutput::Channel, QVector<qint16> > IntMap;

inline void AudioDataOutput::convertAndEmit(const QVector<float> &buffer)
{
    if (m_format == Phonon::Experimental::AudioDataOutput::FloatFormat)
    {
        FloatMap map;
        map.insert(Phonon::Experimental::AudioDataOutput::LeftChannel, buffer);
        map.insert(Phonon::Experimental::AudioDataOutput::RightChannel, buffer);
        emit dataReady(map);
    }
    else
    {
        IntMap map;
        QVector<qint16> intBuffer(m_dataSize);
        for (int i = 0; i < m_dataSize; ++i)
            intBuffer[i] = static_cast<qint16>(buffer[i] * static_cast<float>(0x7FFF));
        map.insert(Phonon::Experimental::AudioDataOutput::LeftChannel, intBuffer);
        map.insert(Phonon::Experimental::AudioDataOutput::RightChannel, intBuffer);
        emit dataReady(map);
    }
}

void AudioDataOutput::processBuffer(QVector<float> &_buffer)
{
    const QVector<float> &buffer(_buffer);
    // TODO emit endOfMedia
    m_pendingData += buffer;
    if (m_pendingData.size() < m_dataSize)
        return;

    if (m_pendingData.size() == m_dataSize)
        convertAndEmit(buffer);
    else
    {
        QVector<float> floatBuffer(m_dataSize);
        while (m_pendingData.size() >= m_dataSize)
        {
            memcpy(floatBuffer.data(), m_pendingData.constData(), m_dataSize * sizeof(float));
            convertAndEmit(floatBuffer);
            int newSize = m_pendingData.size() - m_dataSize;
            memmove(m_pendingData.data(), m_pendingData.constData() + m_dataSize, newSize * sizeof(float));
            m_pendingData.resize(newSize);
        }
    }
}

}} //namespace Phonon::Fake

#include "moc_audiodataoutput.cpp"
// vim: sw=4 ts=4
