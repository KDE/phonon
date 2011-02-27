/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#include "stream.h"
#include "mediaobject.h"

namespace Phonon
{
namespace Fake
{

/* Fake stream:
 * Tries to reach a ratio of 4MB = 4min
 * => 4 000 000 Byte = 240 000 msec
 * => 50 Byte = 3 msec
 * make that 51 Byte = 3 msec => 17 Byte = 1 msec
 */

Stream::Stream(const MediaSource &mediaSource, MediaObject *parent)
    : m_eof(false),
    m_streamSeekable(false),
    m_streamSize(0),
    m_bufferSize(0),
    m_streamPosition(0),
    m_mediaObject(parent)
{
    connectToSource(mediaSource);
}

void Stream::writeData(const QByteArray &data)
{
    Q_ASSERT(!m_eof);
    m_bufferSize += data.size();
    if (m_bufferSize > 17 * 1000) {
        if (m_mediaObject->state() == Phonon::BufferingState) {
            m_mediaObject->setState(Phonon::PlayingState);
        } else if (m_mediaObject->state() == Phonon::LoadingState) {
            m_mediaObject->setState(Phonon::StoppedState);
        }
    } else {
        emit m_mediaObject->bufferStatus(m_bufferSize / 17000);
    }
}

void Stream::endOfData()
{
    m_eof = true;
    if (m_mediaObject->state() == Phonon::BufferingState) {
        m_mediaObject->setState(Phonon::PlayingState);
    } else if (m_mediaObject->state() == Phonon::LoadingState) {
        m_mediaObject->setState(Phonon::StoppedState);
    }
}

void Stream::setStreamSize(qint64 newSize)
{
    m_streamSize = newSize;
    if (m_streamSize > 0) {
        emit m_mediaObject->totalTimeChanged(m_mediaObject->totalTime());
    }
}

void Stream::setStreamSeekable(bool s)
{
    m_streamSeekable = s;
}

void Stream::consumeStream(int msec)
{
    const qint64 bytes = msec * 17;
    if (m_bufferSize < bytes) {
        m_streamPosition += m_bufferSize;
        m_bufferSize = 0;
    } else {
        m_streamPosition += bytes;
        m_bufferSize -= bytes;
    }
}

} // namespace Fake
} // namespace Phonon

//#include "moc_stream.cpp"
// vim: sw=4 sts=4 et tw=100
