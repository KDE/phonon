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

#include "videographicsobject.h"

#include <QtGui/QPainter>

#include "factory_p.h"
#include "medianode_p.h"
#include "phonondefs_p.h"

#define PHONON_INTERFACENAME VideoGraphicsObjectInterface

namespace Phonon {

class VideoGraphicsObjectPrivate : public MediaNodePrivate
{
    Q_DECLARE_PUBLIC(VideoGraphicsObject)
public:
    VideoGraphicsObjectPrivate() :
        geometry(0, 0, 320, 240),
        boundingRect(0, 0, 0, 0),
        frameSize(0, 0)
    {}

    virtual QObject *qObject() { return q_func(); }

    QRectF geometry;
    QRectF boundingRect;
    QSize frameSize;

protected:
    bool aboutToDeleteBackendObject() {}
    void createBackendObject()
    {
        if (m_backendObject)
            return;

        Q_Q(VideoGraphicsObject);
        m_backendObject = Factory::createVideoGraphicsObject(q);
        if (m_backendObject) {
            pINTERFACE_CALL(setVideoGraphicsObject(q));
            QObject::connect(m_backendObject, SIGNAL(frameReady()),
                             q, SLOT(frameReady()),
                             Qt::QueuedConnection);
        }
    }
};

VideoGraphicsObject::VideoGraphicsObject(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    MediaNode(*new VideoGraphicsObjectPrivate)
{
    setFlag(ItemHasNoContents, false);

    K_D(VideoGraphicsObject);
    d->createBackendObject();
}

VideoGraphicsObject::~VideoGraphicsObject()
{
}

QRectF VideoGraphicsObject::boundingRect() const
{
    K_D(const VideoGraphicsObject);
    return d->boundingRect;
}

void VideoGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    K_D(VideoGraphicsObject);
//    qDebug() << Q_FUNC_INFO;
//    qDebug() << boundingRect();

    static bool paintedOnce = false;
    static bool gotSize = false;

    INTERFACE_CALL(lock());

    VideoFrame frame = *INTERFACE_CALL(frame());

    // NOTE: it would be most useful if we had a signal to notify about dimension changes...
    // NOTE: it would be even better if a frame contained a QRectF
    if (frame.format != VideoFrame::Format_Invalid &&
            !frame.qImage().isNull() &&
            !gotSize) {
        gotSize = true;
        d->frameSize = QSize(frame.width, frame.height);
        setTargetRect();
    }

    if (frame.format == VideoFrame::Format_Invalid && !paintedOnce) {
        painter->fillRect(d->geometry, Qt::black);
    } else if (!frame.qImage().isNull()){
        painter->drawImage(d->geometry, frame.qImage());
    }

    INTERFACE_CALL(unlock());

    paintedOnce = true;
}

void VideoGraphicsObject::setGeometry(const QRectF &newGeometry)
{
    K_D(VideoGraphicsObject);
    d->geometry = newGeometry;
}

void VideoGraphicsObject::setTargetRect()
{
    K_D(VideoGraphicsObject);
    emit prepareGeometryChange();

    // keep aspect
    QSizeF frameSize = d->frameSize;
    frameSize.scale(d->targetSize, Qt::KeepAspectRatio);

    float xPosition = (frameSize.width() -1)/2.0;
    float yPosition = (frameSize.height()-1)/2.0;

    QRectF newRect = QRectF(xPosition, yPosition,
                            frameSize.width(), frameSize.height());
//    newRect.moveCenter(QRectF(newRect.topLeft(), m_targetSize).center());

    d->geometry = newRect;
}

void VideoGraphicsObject::frameReady()
{
    K_D(const VideoGraphicsObject);
    update(d->boundingRect);
}

} // namespace Phonon
