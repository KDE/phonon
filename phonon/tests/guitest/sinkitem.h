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


#ifndef SINKITEM_H
#define SINKITEM_H

#include "widgetrectitem.h"
#include <Phonon/MediaNode>

using Phonon::MediaNode;

class SinkItem : public WidgetRectItem
{
    public:
        enum { Type = UserType + 3 };
        int type() const { return Type; }

        virtual MediaNode *mediaNode() = 0;
        virtual const MediaNode *mediaNode() const = 0;

    protected:
        SinkItem(QGraphicsItem *parent, QGraphicsView *widget)
            : WidgetRectItem(parent, widget)
        {}
        SinkItem(const QPoint &pos, QGraphicsView *widget)
            : WidgetRectItem(pos, widget)
        {}
};

#endif // SINKITEM_H
