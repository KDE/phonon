/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#include "bytestream.h"
#include <kdebug.h>
#include <klocale.h>

#include "xineengine.h"
#include "events.h"
#include <QEvent>
#include <QTimer>
#include <cstring>
#include <cstdio>
#include <unistd.h>

extern "C" {
#define this this_xine
#include <xine/input_plugin.h> // needed for MAX_PREVIEW_SIZE
#include <xine/xine_internal.h>
#undef this
}

//#define VERBOSE_DEBUG
#ifdef VERBOSE_DEBUG
#  define PXINE_VDEBUG kDebug(610)
#else
#  define PXINE_VDEBUG kDebugDevNull()
#endif
#define PXINE_DEBUG kDebug(610)

namespace Phonon
{
namespace Xine
{

ByteStream *ByteStream::fromMrl(const QByteArray &mrl)
{
    if (!mrl.startsWith("kbytestream:/")) {
        return 0;
    }
    ByteStream *ret = 0;
    const unsigned int length = mrl.length();
    Q_ASSERT(length >= 13 + sizeof(void *) && length <= 13 + 2 * sizeof(void *));
    const unsigned char *encoded = reinterpret_cast<const unsigned char *>(mrl.constData() + 13);
    unsigned char *addrHack = reinterpret_cast<unsigned char *>(&ret);
    for (unsigned int i = 0; i < sizeof(void *); ++i, ++encoded) {
        if (*encoded == 0x01) {
            ++encoded;
            switch (*encoded) {
            case 0x01:
                addrHack[i] = '\0';
                break;
            case 0x02:
                addrHack[i] = '\1';
                break;
            case 0x03:
                addrHack[i] = '#';
                break;
            case 0x04:
                addrHack[i] = '%';
                break;
            default:
                abort();
            }
        } else {
            addrHack[i] = *encoded;
        }
    }
    return ret;
}

ByteStream::ByteStream(const MediaSource &mediaSource, MediaObject *parent)
    : QObject(0), // don't let MediaObject's ~QObject delete us - the input plugin will delete us
    m_mediaObject(parent),
    m_streamSize(0),
    m_currentPosition(0),
    m_buffersize(0),
    m_offset(0),
    m_seekable(false),
    m_stopped(false),
    m_eod(false),
    m_buffering(false),
    m_firstReset(true)
{
    connect(this, SIGNAL(resetQueued()), this, SLOT(callStreamInterfaceReset()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(needDataQueued()), this, SLOT(needData()), Qt::QueuedConnection);
    connect(this, SIGNAL(seekStreamQueued(qint64)), this, SLOT(syncSeekStream(qint64)), Qt::QueuedConnection);

    connectToSource(mediaSource);

    // created in the main thread
    m_mainThread = pthread_self();
}

void ByteStream::pullBuffer(char *buf, int len)
{
    if (m_stopped) {
        return;
    }
    // never called from main thread
    //Q_ASSERT(m_mainThread != pthread_self());

    PXINE_VDEBUG << len << ", m_offset = " << m_offset << ", m_currentPosition = "
        << m_currentPosition << ", m_buffersize = " << m_buffersize;
    while (len > 0) {
        if (m_buffers.isEmpty()) {
            // pullBuffer is only called when there's => len data available
            kFatal(610) << "m_currentPosition = " << m_currentPosition
                << ", m_preview.size() = " << m_preview.size() << ", len = " << len << kBacktrace();
        }
        if (m_buffers.head().size() - m_offset <= len) {
            // The whole data of the next buffer is needed
            QByteArray buffer = m_buffers.dequeue();
            PXINE_VDEBUG << "dequeue one buffer of size " << buffer.size()
                << ", reading at offset = " << m_offset << ", resetting m_offset to 0";
            Q_ASSERT(buffer.size() > 0);
            int tocopy = buffer.size() - m_offset;
            Q_ASSERT(tocopy > 0);
            xine_fast_memcpy(buf, buffer.constData() + m_offset, tocopy);
            buf += tocopy;
            len -= tocopy;
            Q_ASSERT(len >= 0);
            Q_ASSERT(m_buffersize >= static_cast<size_t>(tocopy));
            m_buffersize -= tocopy;
            m_offset = 0;
        } else {
            // only a part of the next buffer is needed
            PXINE_VDEBUG << "read " << len
                << " bytes from the first buffer at offset = " << m_offset;
            QByteArray &buffer = m_buffers.head();
            Q_ASSERT(buffer.size() > 0);
            xine_fast_memcpy(buf, buffer.constData() + m_offset , len);
            m_offset += len;
            Q_ASSERT(m_buffersize >= static_cast<size_t>(len));
            m_buffersize -= len;
            len = 0;
        }
    }
}

int ByteStream::peekBuffer(void *buf)
{
    if (m_stopped) {
        return 0;
    }

    // never called from main thread
    //Q_ASSERT(m_mainThread != pthread_self());

    if (m_preview.size() < MAX_PREVIEW_SIZE && !m_eod) {
        QMutexLocker lock(&m_mutex);
        // the thread needs to sleep until a wait condition is signalled from writeData
        while (!m_eod && !m_stopped && m_preview.size() < MAX_PREVIEW_SIZE) {
            PXINE_VDEBUG << "xine waits for data: " << m_buffersize << ", " << m_eod;
            emit needDataQueued();
            m_waitingForData.wait(&m_mutex);
        }
        if (m_stopped) {
            PXINE_DEBUG << "returning 0, m_stopped = true";
            return 0;
        }
    }

    xine_fast_memcpy(buf, m_preview.constData(), m_preview.size());
    return m_preview.size();
}

qint64 ByteStream::readFromBuffer(void *buf, size_t count)
{
    if (m_stopped) {
        return 0;
    }
    // never called from main thread
    //Q_ASSERT(m_mainThread != pthread_self());

    qint64 currentPosition = m_currentPosition;

    PXINE_VDEBUG << count;

    QMutexLocker lock(&m_mutex);
    //kDebug(610) << "LOCKED m_mutex: ";
    // get data while more is needed and while we're still receiving data
    if (m_buffersize < count && !m_eod) {
        // the thread needs to sleep until a wait condition is signalled from writeData
        while (!m_eod && !m_stopped && m_buffersize < count) {
            PXINE_VDEBUG << "xine waits for data: " << m_buffersize << ", " << m_eod;
            emit needDataQueued();
            m_waitingForData.wait(&m_mutex);
        }
        if (m_stopped) {
            PXINE_DEBUG << "returning 0, m_stopped = true";
            //kDebug(610) << "UNLOCKING m_mutex: ";
            return 0;
        }
        Q_ASSERT(currentPosition == m_currentPosition);
        //PXINE_VDEBUG << "m_buffersize = " << m_buffersize;
    }
    if (m_buffersize >= count) {
        PXINE_VDEBUG << "calling pullBuffer with m_buffersize = " << m_buffersize;
        pullBuffer(static_cast<char *>(buf), count);
        m_currentPosition += count;
        //kDebug(610) << "UNLOCKING m_mutex: ";
        return count;
    }
    Q_ASSERT(m_eod);
    if (m_buffersize > 0) {
        PXINE_VDEBUG << "calling pullBuffer with m_buffersize = " << m_buffersize;
        const int len = m_buffersize;
        pullBuffer(static_cast<char *>(buf), len);
        m_currentPosition += len;
        PXINE_DEBUG << "returning less data than requested, the stream is at its end";
        //kDebug(610) << "UNLOCKING m_mutex: ";
        return len;
    }
    PXINE_DEBUG << "return 0, the stream is at its end";
    //kDebug(610) << "UNLOCKING m_mutex: ";
    return 0;
}

off_t ByteStream::seekBuffer(qint64 offset)
{
    if (m_stopped) {
        return 0;
    }
    // never called from main thread
    //Q_ASSERT(m_mainThread != pthread_self());

    // no seek
    if (offset == m_currentPosition) {
        return m_currentPosition;
    }

    // impossible seek
    if (offset > m_streamSize) {
        kWarning(610) << "xine is asking to seek behind the end of the data stream";
        return m_currentPosition;
    }

    // first try to seek in the data we have buffered
    m_mutex.lock();
    //kDebug(610) << "LOCKED m_mutex: ";
    if (offset > m_currentPosition && offset < m_currentPosition + m_buffersize) {
        kDebug(610) << "seeking behind current position, but inside the buffered data";
        // seek behind the current position in the buffer
        while (offset > m_currentPosition) {
            const int gap = offset - m_currentPosition;
            Q_ASSERT(!m_buffers.isEmpty());
            const int buffersize = m_buffers.head().size() - m_offset;
            if (buffersize <= gap) {
                // discard buffers if they hold data before offset
                Q_ASSERT(!m_buffers.isEmpty());
                QByteArray buffer = m_buffers.dequeue();
                m_buffersize -= buffersize;
                m_currentPosition += buffersize;
                m_offset = 0;
            } else {
                // offset points to data in the next buffer
                m_buffersize -= gap;
                m_currentPosition += gap;
                m_offset += gap;
            }
        }
        Q_ASSERT(offset == m_currentPosition);
        //kDebug(610) << "UNLOCKING m_mutex: ";
        m_mutex.unlock();
        return m_currentPosition;
    } else if (offset < m_currentPosition && m_currentPosition - offset <= m_offset) {
        kDebug(610) << "seeking in current buffer: m_currentPosition = " << m_currentPosition << ", m_offset = " << m_offset;
        // seek before the current position in the buffer
        m_offset -= m_currentPosition - offset;
        m_buffersize += m_currentPosition - offset;
        Q_ASSERT(m_offset >= 0);
        m_currentPosition = offset;
        //kDebug(610) << "UNLOCKING m_mutex: ";
        m_mutex.unlock();
        return m_currentPosition;
    }

    // the ByteStream is not seekable: no chance to seek to the requested offset
    if (!m_seekable) {
        //kDebug(610) << "UNLOCKING m_mutex: ";
        m_mutex.unlock();
        return m_currentPosition;
    }

    PXINE_DEBUG << "seeking to a position that's not in the buffered data: clear the buffer. "
        " new offset = " << offset <<
        ", m_buffersize = " << m_buffersize <<
        ", m_offset = " << m_offset <<
        ", m_eod = " << m_eod <<
        ", m_currentPosition = " << m_currentPosition;

    // throw away the buffers and ask for new data
    m_buffers.clear();
    m_buffersize = 0;
    m_offset = 0;
    m_eod = false;

    m_currentPosition = offset;
    //kDebug(610) << "UNLOCKING m_mutex: ";
    m_mutex.unlock();

    QMutexLocker seekLock(&m_seekMutex);
    if (m_stopped) {
        return 0;
    }
    emit seekStreamQueued(offset); //calls syncSeekStream from the main thread
    m_seekWaitCondition.wait(&m_seekMutex); // waits until the seekStream signal returns
    return offset;
}

off_t ByteStream::currentPosition() const
{
    return m_currentPosition;
}

ByteStream::~ByteStream()
{
    Q_ASSERT(m_mainThread == pthread_self());
    PXINE_DEBUG;
}

QByteArray ByteStream::mrl() const
{
    QByteArray mrl("kbytestream:/");
    // the address can contain 0s which will null-terminate the C-string
    // use a simple encoding: 0x00 -> 0x0101, 0x01 -> 0x0102
    const ByteStream *iface = this;
    const unsigned char *that = reinterpret_cast<const unsigned char *>(&iface);
    for(unsigned int i = 0; i < sizeof(void *); ++i) {
        switch (that[i]) {
        case 0: // escape 0 as it terminates the string
            mrl += 0x01;
            mrl += 0x01;
            break;
        case 1: // escape 1 because it is used for escaping
            mrl += 0x01;
            mrl += 0x02;
            break;
        case '#': // escape # because xine splits the mrl at #s
            mrl += 0x01;
            mrl += 0x03;
            break;
        case '%': // escape % because xine will replace e.g. %20 with ' '
            mrl += 0x01;
            mrl += 0x04;
            break;
        default:
            mrl += that[i];
        }
    }
    mrl += '\0';
    return mrl;
}

void ByteStream::setStreamSize(qint64 x)
{
    PXINE_VDEBUG << x;
    QMutexLocker lock(&m_streamSizeMutex);
    m_streamSize = x;
    if (m_streamSize != 0) {
        emit needDataQueued();
        m_waitForStreamSize.wakeAll();
    }
}

void ByteStream::setPauseForBuffering(bool b)
{
    if (b) {
        QCoreApplication::postEvent(m_mediaObject->stream().data(), new QEVENT(PauseForBuffering));
        m_buffering = true;
    } else {
        QCoreApplication::postEvent(m_mediaObject->stream().data(), new QEVENT(UnpauseForBuffering));
        m_buffering = false;
    }
}

void ByteStream::endOfData()
{
    PXINE_DEBUG;

    m_mutex.lock();
    m_seekMutex.lock();
    m_streamSizeMutex.lock();
    m_eod = true;
    // don't reset the XineStream because many demuxers hit eod while trying to find the format of
    // the data
    // stream().setMrl(mrl());
    m_seekWaitCondition.wakeAll();
    m_seekMutex.unlock();
    m_waitingForData.wakeAll();
    m_mutex.unlock();
    m_waitForStreamSize.wakeAll();
    m_streamSizeMutex.unlock();
}

void ByteStream::setStreamSeekable(bool seekable)
{
    m_seekable = seekable;
}

void ByteStream::writeData(const QByteArray &data)
{
    if (data.size() <= 0) {
        return;
    }

    // first fill the preview buffer
    if (m_preview.size() != MAX_PREVIEW_SIZE) {
        PXINE_DEBUG << "fill preview";
        // more data than the preview buffer needs
        if (m_preview.size() + data.size() > MAX_PREVIEW_SIZE) {
            int tocopy = MAX_PREVIEW_SIZE - m_preview.size();
            m_preview += data.left(tocopy);
        } else { // all data fits into the preview buffer
            m_preview += data;
        }

        PXINE_VDEBUG << "filled preview buffer to " << m_preview.size();
    }

    PXINE_VDEBUG << data.size() << " m_streamSize = " << m_streamSize;

    QMutexLocker lock(&m_mutex);
    //kDebug(610) << "LOCKED m_mutex: ";
    m_buffers.enqueue(data);
    m_buffersize += data.size();
    PXINE_VDEBUG << "m_buffersize = " << m_buffersize;
    // FIXME accessing m_mediaObject is not threadsafe
    switch (m_mediaObject->state()) {
    case Phonon::BufferingState: // if nbc is buffering we want more data
    case Phonon::LoadingState: // if the preview is not ready we want me more data
        break;
    default:
        enoughData(); // else it's enough
    }
    m_waitingForData.wakeAll();
    //kDebug(610) << "UNLOCKING m_mutex: ";
}

void ByteStream::callStreamInterfaceReset()
{
    StreamInterface::reset();
}

void ByteStream::syncSeekStream(qint64 offset)
{
    PXINE_VDEBUG;
    m_seekMutex.lock();
    seekStream(offset);
    m_seekWaitCondition.wakeAll();
    m_seekMutex.unlock();
}

qint64 ByteStream::streamSize() const
{
    if (m_streamSize == 0) {
        // stream size has not been set yet
        QMutexLocker lock(&m_streamSizeMutex);
        if (m_streamSize == 0 && !m_eod) {
            m_waitForStreamSize.wait(&m_streamSizeMutex);
        }
    }
    return m_streamSize;
}

void ByteStream::stop()
{
    PXINE_VDEBUG;

    m_mutex.lock();
    m_seekMutex.lock();
    m_streamSizeMutex.lock();
    m_stopped = true;
    // the other thread is now not between m_mutex.lock() and m_waitingForData.wait(&m_mutex), so it
    // won't get stuck in m_waitingForData.wait if it's not there right now
    m_seekWaitCondition.wakeAll();
    m_seekMutex.unlock();
    m_waitingForData.wakeAll();
    m_mutex.unlock();
    m_waitForStreamSize.wakeAll();
    m_streamSizeMutex.unlock();
}

void ByteStream::reset()
{
    if (m_firstReset) {
        kDebug(610) << "first reset";
        m_firstReset = false;
        return;
    }
    kDebug(610);
    emit resetQueued();
    m_currentPosition = 0;
    m_buffersize = 0;
    m_offset = 0;
    m_stopped = false;
    m_eod = false;
    m_buffering = false;
    if (m_streamSize != 0) {
        emit needDataQueued();
    }
}

}} //namespace Phonon::Xine

#include "bytestream.moc"
// vim: sw=4 ts=4
