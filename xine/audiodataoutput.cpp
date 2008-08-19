/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "audiodataoutput.h"
#include <QVector>
#include <QMap>
#include "sourcenode.h"

namespace Phonon
{
namespace Xine
{
class AudioDataOutputXT : public SinkNodeXT
{
    void rewireTo(SourceNodeXT *);
};

AudioDataOutput::AudioDataOutput(QObject *parent)
    : AbstractAudioOutput(new AudioDataOutputXT, parent)
{
}

AudioDataOutput::~AudioDataOutput()
{
}

void AudioDataOutputXT::rewireTo(SourceNodeXT *source)
{
    //xine_post_wire_audio_port(source->outputPort(), m_audioPort);
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
    if (m_format == Phonon::Experimental::AudioDataOutput::FloatFormat) {
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

}} //namespace Phonon::Xine

#include "audiodataoutput.moc"
// vim: sw=4 ts=4
