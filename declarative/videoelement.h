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

#ifndef VIDEOELEMENT_H
#define VIDEOELEMENT_H

#include <QtCore/QMutex>
#include <QtDeclarative/QDeclarativeItem>

#include <phonon/experimental/videoframe2.h>

#include "abstractmediaelement.h"

namespace Phonon {

namespace Experimental
{
class VideoDataOutput2;
}

namespace Declarative {

class VideoElement : public QDeclarativeItem, public MediaElement
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    VideoElement(QDeclarativeItem *parent = 0);
    ~VideoElement();

    void classBegin() {};
    void componentComplete() {};

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

signals:
    void sourceChanged();

public slots:
    void play();
    void stop();

private slots:
    void setFrame(const Phonon::Experimental::VideoFrame2 &frame);

private:
    void init();

    Experimental::VideoDataOutput2 *m_videoDataOutput;
    Experimental::VideoFrame2  m_frame;

    QMutex m_mutex;
    QRectF m_rect;
    QSize m_frameSize;
};

} // namespace Declarative
} // namespace Phonon

#endif // VIDEOELEMENT_H
