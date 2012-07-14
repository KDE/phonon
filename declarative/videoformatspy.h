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

#ifndef PHONON_DECLARATIVE_VIDEOFORMATSPY_H
#define PHONON_DECLARATIVE_VIDEOFORMATSPY_H

#include <QtDeclarative/QDeclarativeItem>

#include <phonon/graphicsview/videographicsobject.h>

namespace Phonon {
namespace Declarative {

class VideoFormatSpyElement : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit VideoFormatSpyElement(QDeclarativeItem *parent = 0);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    inline QMap<GraphicsPainterType, QList<VideoFrame::Format> > formats() { return m_formats; }

signals:
    void formatsChanged(QMap<GraphicsPainterType, QList<VideoFrame::Format> > formats);

private:
     QMap<GraphicsPainterType, QList<VideoFrame::Format> > m_formats;
};

} // namespace Declarative
} // namespace Phonon

QML_DECLARE_TYPE(Phonon::Declarative::VideoFormatSpyElement)

#endif // PHONON_DECLARATIVE_VIDEOFORMATSPY_H
