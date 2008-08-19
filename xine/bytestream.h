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
#ifndef PHONON_XINE_BYTESTREAM_H
#define PHONON_XINE_BYTESTREAM_H

#include "mediaobject.h"

#include <xine.h>

#include "xineengine.h"
#include <Phonon/StreamInterface>
#include <QByteArray>
#include <QSharedData>
#include <QQueue>
#include <kdebug.h>
#include <QCoreApplication>
#include <QMutex>
#include <QWaitCondition>
#include <pthread.h>
#include <cstdlib>
#include <QObject>

extern const char Error__off_t_needs_to_have_64_bits[sizeof(off_t) == 8 ? 1 : -1];

namespace Phonon
{
namespace Xine
{
class MediaObject;
class ByteStream : public QObject, public StreamInterface, public QSharedData
{
    Q_OBJECT
    public:
        static ByteStream *fromMrl(const QByteArray &mrl);
        ByteStream(const MediaSource &, MediaObject *parent);
        ~ByteStream();

        QByteArray mrl() const;

        // does not block, but might change later
        bool streamSeekable() const { return m_seekable; }

        // blocks until the size is known
        qint64 streamSize() const;

        void stop();

        void reset();

    public slots:
        void writeData(const QByteArray &data);
        void endOfData();
        void setStreamSeekable(bool);
        void setStreamSize(qint64);

        void setPauseForBuffering(bool);

        // for the xine input plugin:
        int peekBuffer(void *buf);
        qint64 readFromBuffer(void *buf, size_t count);
        off_t seekBuffer(qint64 offset);
        off_t currentPosition() const;

    signals:
        void resetQueued();
        void needDataQueued();
        void seekStreamQueued(qint64);

    private slots:
        void callStreamInterfaceReset();
        void syncSeekStream(qint64 offset);
        void needData() { StreamInterface::needData(); }

    private:
//X             void setMrl();
        void pullBuffer(char *buf, int len);

        MediaObject *m_mediaObject;
        QByteArray m_preview;
        QMutex m_mutex;
        QMutex m_seekMutex;
        mutable QMutex m_streamSizeMutex;
        mutable QWaitCondition m_waitForStreamSize;
        QWaitCondition m_waitingForData;
        QWaitCondition m_seekWaitCondition;
        QQueue<QByteArray> m_buffers;

        pthread_t m_mainThread;
        qint64 m_streamSize;
        qint64 m_currentPosition;
        size_t m_buffersize;
        int m_offset;

        bool m_seekable : 1;
        bool m_stopped : 1;
        bool m_eod : 1;
        bool m_buffering : 1;
        bool m_firstReset : 1;
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4 sts=4 et tw=100
#endif // PHONON_XINE_BYTESTREAM_H
