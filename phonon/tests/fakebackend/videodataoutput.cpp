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

#include "videodataoutput.h"

namespace Phonon
{
namespace Fake
{
VideoDataOutput::VideoDataOutput(QObject *parent)
    : QObject(parent)
{
}

VideoDataOutput::~VideoDataOutput()
{
}

//X int VideoDataOutput::displayLatency() const
//X {
//X     return m_latency;
//X }

int VideoDataOutput::frameRate() const
{
    return m_frameRate;
}

void VideoDataOutput::setFrameRate(int frameRate)
{
    m_frameRate = frameRate;
}

QSize VideoDataOutput::naturalFrameSize() const
{
    return QSize(320, 240);
}

QSize VideoDataOutput::frameSize() const
{
    return m_frameSize;
}

void VideoDataOutput::setFrameSize(const QSize &frameSize)
{
    m_frameSize = frameSize;
}

//X void VideoDataOutput::setDisplayLatency(int milliseconds)
//X {
//X     m_latency = milliseconds;
//X }

void VideoDataOutput::processFrame(Phonon::Experimental::VideoFrame &frame)
{
    emit frameReady(frame);
    // TODO emit endOfMedia
}

}} //namespace Phonon::Fake

#include "moc_videodataoutput.cpp"
// vim: sw=4 ts=4
