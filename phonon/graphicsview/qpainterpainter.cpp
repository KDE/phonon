/*
    Copyright (C) 2012 Harald Sitter <sitter@kde.org>

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

#include "qpainterpainter.h"

#include <QtGui/QImage>
#include <QtGui/QPainter>

namespace Phonon {

QPainterPainter::QPainterPainter()
{
}

QPainterPainter:: ~QPainterPainter()
{
}

QList<VideoFrame::Format> QPainterPainter::supportedFormats() const
{
    return QList<VideoFrame::Format>() << VideoFrame::Format_RGB32;
}

void QPainterPainter::init()
{
    m_inited = true;
}

void QPainterPainter::paint(QPainter *painter, QRectF target)
{
    // QImage can only handle packed formats.
    if (m_frame->planeCount != 1 || m_frame->format != VideoFrame::Format_RGB32) {
        painter->drawImage(target, QImage());
    } else {
        painter->drawImage(target,
                           QImage(reinterpret_cast<const uchar *>(m_frame->plane[0].constData()),
                                  m_frame->width, m_frame->height,
                                  QImage::Format_RGB32));
    }
}

} // namespace Phonon
