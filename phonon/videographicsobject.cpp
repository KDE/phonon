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

namespace Phonon {

VideoGraphicsObject::VideoGraphicsObject(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
}

VideoGraphicsObject::~VideoGraphicsObject()
{
}

void VideoGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    qDebug() << Q_FUNC_INFO;
//    qDebug() << boundingRect();
    static quint64 c = 0;
    static bool paintedOnce = false;
    static bool gotSize = false;

    VideoFrame frame;
    m_mutex.lock();
    frame = m_frame;
    m_mutex.unlock();

    // NOTE: it would be most useful if we had a signal to notify about dimension changes...
    // NOTE: it would be even better if a frame contained a QRectF
    if (frame.format != VideoFrame::Format_Invalid &&
            !frame.qImage().isNull() &&
            !gotSize) {
        gotSize = true;
        m_frameSize = QSize(frame.width, frame.height);
//        setTargetRect();
//        qDebug() << frame.width;
//        qDebug() << frame.height;
    }

//    if (frame.format != VideoFrame2::Format_Invalid &&
//            !frame.qImage().isNull() && // Validity does not always mean the frame has sensible dimensions...
//            (m_rect.width() != frame.width ||
//             m_rect.height() != frame.height)) {
//        emit prepareGeometryChange();
//        m_rect = QRectF(0,0,frame.width, frame.height);
////        setTargetRect();
//        qDebug() << frame.width;
//        qDebug() << frame.height;
//    }

    if (frame.format == VideoFrame::Format_Invalid && !paintedOnce) {
        painter->fillRect(m_rect, Qt::black);
        c++;
    } else if (!frame.qImage().isNull()){
        painter->drawImage(m_rect, frame.qImage());
        c++;
    } else if (frame.format == VideoFrame::Format_Invalid) {
//        qDebug() << c++ << ": INVALID";
    } else if (frame.qImage().isNull()) {
//        qDebug() << c++ << ": NULL";
    }
//    qDebug() << "P" << c;
    paintedOnce = true;
}

void VideoGraphicsObject::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_rect=newGeometry;
}

//void VideoGraphicsObject::setTargetRect()
//{
//    emit prepareGeometryChange();

//    // keerp aspect
//    QSizeF size = m_frameSize;
//    size.scale(m_targetSize, Qt::KeepAspectRatio);

//    QRectF newRect = QRectF(0, 0, size.width(), size.height());
////    newRect.moveCenter(QRectF(newRect.topLeft(), m_targetSize).center());

//    m_rect = newRect;
//}


void VideoGraphicsObject::setFrame(const VideoFrame &frame)
{
    m_mutex.lock();
    m_frame = frame;
    m_mutex.unlock();

    update();
}

} // namespace Phonon
