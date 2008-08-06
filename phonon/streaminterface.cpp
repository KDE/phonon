/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "streaminterface.h"
#include "streaminterface_p.h"
#include "abstractmediastream2.h"
#include "abstractmediastream_p.h"
#include "mediasource_p.h"
#include "phononnamespace_p.h"
#include <QtCore/QEvent>

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM

QT_BEGIN_NAMESPACE

namespace Phonon
{

StreamInterface::StreamInterface()
    : d(new StreamInterfacePrivate)
{
    d->q = this;
}

StreamInterface::~StreamInterface()
{
    if (d->streamEventQueue) {
        // better be safe and make the AbstractMediaStream2 not fill the queue uselessly
        d->streamEventQueue->sendToFrontend(StreamEventQueue::EnoughData);

        // setBackendCommandHandler blocks internally until a handler that might be called right
        // now is done. That means StreamInterfacePrivate::dataReady will either not run again
        // or it will be done before this object and our d object are deleted.
        d->streamEventQueue->setBackendCommandHandler(0);
        d->streamEventQueue->deref();
        d->streamEventQueue = 0;
    } else if (d->connected) {
        AbstractMediaStreamPrivate *dd = d->mediaSource.stream()->d_func();
        dd->setStreamInterface(0);
    }
    delete d;
    // if eventDispatcher->wakeUp() was called and we are now still connected to its awake() signal
    // then the QObject will be deleted until execution reaches the event loop again and
    // handleStreamEvent will not get called
}

void StreamInterface::connectToSource(const MediaSource &mediaSource)
{
    Q_ASSERT(!d->connected);
    Q_ASSERT(!d->streamEventQueue);
    d->mediaSource = mediaSource;
    Q_ASSERT(d->mediaSource.type() == MediaSource::Stream);
    Q_ASSERT(d->mediaSource.stream());
#if 0
    AbstractMediaStream2 *stream2 = qobject_cast<AbstractMediaStream2 *>(d->mediaSource.stream());
    if (stream2) {
        if (!d->qobject) {
            d->qobject = new StreamInterfacePrivateHelper(d);
        }
        if (!d->awakeSignalConnected) {
            d->awakeSignalConnected = true;
            QObject::connect(d->eventDispatcher, SIGNAL(awake()), d->qobject, SLOT(_k_handleStreamEvent()), Qt::DirectConnection);
        }
        d->streamEventQueue = mediaSource.d->streamEventQueue;
        d->streamEventQueue->ref();
        d->streamEventQueue->setBackendCommandHandler(d);
        d->streamEventQueue->sendToFrontend(StreamEventQueue::Connect);
    } else {
#endif
        if (d->awakeSignalConnected) {
            d->awakeSignalConnected = false;
            QObject::disconnect(d->eventDispatcher, SIGNAL(awake()), d->qobject, SLOT(_k_handleStreamEvent()));
        }
        d->connected = true;
        AbstractMediaStreamPrivate *dd = d->mediaSource.stream()->d_func();
        dd->setStreamInterface(this);
        d->mediaSource.stream()->reset();
#if 0
    }
#endif
}

void StreamInterfacePrivate::disconnectMediaStream()
{
    Q_ASSERT(connected);
    Q_ASSERT(!streamEventQueue);
    connected = false;

    // if mediaSource has autoDelete set then it will delete the AbstractMediaStream again who's
    // destructor is calling us right now
    mediaSource.setAutoDelete(false);

    mediaSource = MediaSource();
    q->endOfData();
    q->setStreamSeekable(false);
}

void StreamInterface::needData()
{
    if (d->streamEventQueue) {
        d->streamEventQueue->sendToFrontend(StreamEventQueue::NeedData, quint32(4096));
    } else if (d->connected) {
        d->mediaSource.stream()->needData();
    }
}

void StreamInterface::enoughData()
{
    if (d->streamEventQueue) {
        d->streamEventQueue->sendToFrontend(StreamEventQueue::EnoughData);
    } else {
        Q_ASSERT(d->connected);
        d->mediaSource.stream()->enoughData();
    }
}

void StreamInterface::seekStream(qint64 offset)
{
    if (d->streamEventQueue) {
        d->streamEventQueue->sendToFrontend(StreamEventQueue::Seek, offset);
    } else {
        Q_ASSERT(d->connected);
        d->mediaSource.stream()->seekStream(offset);
    }
}

void StreamInterface::reset()
{
    if (d->streamEventQueue) {
        d->streamEventQueue->sendToFrontend(StreamEventQueue::Reset);
    } else {
        Q_ASSERT(d->connected);
        d->mediaSource.stream()->reset();
    }
}

void StreamInterfacePrivate::dataReady()
{
    Q_ASSERT(eventDispatcher);
    eventDispatcher->wakeUp();
}

void StreamInterfacePrivate::_k_handleStreamEvent()
{
    Q_ASSERT(streamEventQueue);
    StreamEventQueue::Command c;
    while (streamEventQueue->nextCommandForBackend(&c)) {
        switch (c.command) {
        case StreamEventQueue::NeedData:
        case StreamEventQueue::EnoughData:
        case StreamEventQueue::Seek:
        case StreamEventQueue::Reset:
        case StreamEventQueue::Connect:
        case StreamEventQueue::SeekDone:
        case StreamEventQueue::ResetDone:
        case StreamEventQueue::ConnectDone:
            pFatal("AbstractMediaStream2 received wrong Command");
            break;
        case StreamEventQueue::SetStreamSize:
            q->setStreamSize(qvariant_cast<quint64>(c.data));
            break;
        case StreamEventQueue::SetSeekable:
            q->setStreamSeekable(qvariant_cast<bool>(c.data));
            break;
        case StreamEventQueue::Write:
            q->writeData(qvariant_cast<QByteArray>(c.data));
            break;
        case StreamEventQueue::EndOfData:
            q->endOfData();
            break;
        }
    }
}

#if 0
void StreamInterface2::needData(quint32 size)
{
    if (d->streamEventQueue) {
        d->streamEventQueue->sendToFrontend(StreamEventQueue::NeedData, size);
    } else if (d->connected) {
        d->mediaSource.stream()->needData();
    }
}

StreamInterface2::StreamInterface2(QObject *parent)
    : QObject(parent)
{
    d->qobject = this;
}

StreamInterface2::~StreamInterface2()
{
    d->qobject = 0;
}
#endif

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_streaminterface.cpp"
#include "moc_streaminterface_p.cpp"

#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
