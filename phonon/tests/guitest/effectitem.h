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

#ifndef EFFECTITEM_H
#define EFFECTITEM_H

#include "sinkitem.h"
#include <Phonon/Effect>
#include <Phonon/EffectDescription>

class QModelIndex;
class PathItem;

using Phonon::Effect;
using Phonon::EffectDescription;

class EffectItem : public SinkItem
{
    Q_OBJECT
    public:
        EffectItem(const EffectDescription &, PathItem *pathItem, QGraphicsView *widget);
        EffectItem(const EffectDescription &, const QPoint &pos, QGraphicsView *widget);

        enum { Type = UserType + 10 };
        int type() const { return Type; }

        virtual MediaNode *mediaNode() { return &m_effect; }
        virtual const MediaNode *mediaNode() const { return &m_effect; }

    private:
        void setupUi(const EffectDescription &desc);

        Effect m_effect;
};

#endif // EFFECTITEM_H
