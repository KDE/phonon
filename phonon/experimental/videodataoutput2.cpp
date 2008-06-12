/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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

#include "videodataoutput2.h"
#include "videodataoutput2_p.h"
#include "../factory_p.h"
#include <QtCore/QSize>
#include <QtCore/QThread>

#define PHONON_CLASSNAME VideoDataOutput2

namespace Phonon
{
namespace Experimental
{

VideoDataOutput2::VideoDataOutput2(QObject *parent)
    : QObject(parent),
    AbstractVideoDataOutput(*new VideoDataOutput2Private)
{
}

void VideoDataOutput2::frameReady(const VideoFrame2 &frame)
{
    if (QThread::currentThread() == thread()) {
        emit frameReadySignal(frame);
    } else {
        QMetaObject::invokeMethod(this, "frameReadySignal", Qt::BlockingQueuedConnection, Q_ARG(VideoFrame2, frame));
    }
}

void VideoDataOutput2::endOfMedia()
{
    if (QThread::currentThread() == thread()) {
        emit endOfMediaSignal();
    } else {
        QMetaObject::invokeMethod(this, "endOfMediaSignal", Qt::BlockingQueuedConnection);
    }
}

void VideoDataOutput2Private::createBackendObject()
{
    AbstractVideoDataOutputPrivate::createBackendObject();
}

bool VideoDataOutput2Private::aboutToDeleteBackendObject()
{
    return AbstractVideoDataOutputPrivate::aboutToDeleteBackendObject();
}

void VideoDataOutput2Private::setupBackendObject()
{
    AbstractVideoDataOutputPrivate::setupBackendObject();
}

} // namespace Experimental
} // namespace Phonon

#include "videodataoutput2.moc"

#undef PHONON_CLASSNAME
// vim: sw=4 ts=4 tw=80
