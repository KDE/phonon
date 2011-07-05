/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#include "videoelement.h"

#include <QtGui/QPainter>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/experimental/videodataoutput2.h>

using namespace Phonon::Experimental;

namespace Phonon {
namespace Declarative {

#warning there might be awful problems when not deriving from qdeclarativeitem!!! (virtual inheritance?)

VideoElement::VideoElement(QGraphicsItem *parent) :
    VideoGraphicsObject(parent)
{
}

VideoElement::~VideoElement()
{
}

void VideoElement::onFrameReady(Phonon::Experimental::VideoFrame2 frame)
{
    VideoFrame f;
    f.aspectRatio = frame.aspectRatio;
    f.data0 = frame.data0;
    f.data1 = frame.data1;
    f.data2 = frame.data2;
    f.format = (VideoFrame::Format)(int)frame.format;
    f.height = frame.height;
    f.width = frame.width;
    setFrame(f);
}

void VideoElement::init(MediaObject *mediaObject)
{
    Q_ASSERT(mediaObject);
    if (m_videoDataOutput)
        return;

    m_mediaObject = mediaObject;

    m_videoDataOutput = new VideoDataOutput2(this);
    createPath(m_mediaObject, m_videoDataOutput);

    connect(m_videoDataOutput, SIGNAL(frameReadySignal(Phonon::Experimental::VideoFrame2)),
            SLOT(onFrameReady(Phonon::Experimental::VideoFrame2)));
    m_videoDataOutput->start();

//    connect(this, SIGNAL(), vgo, SLOT(setTargetSize(QSize)));
}

} // namespace Declarative
} // namespace Phonon
