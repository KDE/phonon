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

#ifndef VIDEOWIDGETITEM_H
#define VIDEOWIDGETITEM_H

#include "sinkitem.h"
#include <QtGui/QVBoxLayout>
#include <Phonon/VideoWidget>

using Phonon::VideoWidget;

class VideoWidgetItem : public SinkItem
{
    public:
        VideoWidgetItem(const QPoint &pos, QGraphicsView *widget)
            : SinkItem(pos, widget)
        {
            setBrush(QColor(100, 100, 255, 150));
            setTitle("Video Widget");

            QVBoxLayout *layout = new QVBoxLayout(m_frame);
            layout->setMargin(0);
            m_videoWidget = new VideoWidget(m_frame);
            m_videoWidget->setMinimumSize(160, 90);
            layout->addWidget(m_videoWidget);
        }

        //enum { Type = UserType + 5 };
        //int type() const { return Type; }

        virtual MediaNode *mediaNode() { return m_videoWidget; }
        virtual const MediaNode *mediaNode() const { return m_videoWidget; }

    private:
        VideoWidget *m_videoWidget;
};

#endif // VIDEOWIDGETITEM_H
