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

#include "effectitem.h"
#include <QtCore/QModelIndex>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>

#include <Phonon/BackendCapabilities>
#include <Phonon/EffectWidget>
#include "pathitem.h"

using Phonon::EffectWidget;

EffectItem::EffectItem(const EffectDescription &desc, PathItem *pathItem, QGraphicsView *widget)
    : SinkItem(pathItem, widget),
    m_effect(desc)
{
    setupUi(desc);
    pathItem->path().insertEffect(&m_effect);
    pathItem->updateChildrenPositions();
}

EffectItem::EffectItem(const EffectDescription &desc, const QPoint &pos, QGraphicsView *widget)
    : SinkItem(pos, widget),
    m_effect(desc)
{
    setupUi(desc);
}

void EffectItem::setupUi(const EffectDescription &desc)
{
    setBrush(QColor(255, 200, 0, 150));
    setTitle(desc.name());

    QVBoxLayout *hlayout = new QVBoxLayout(m_frame);
    hlayout->setMargin(0);

    QLabel *label = new QLabel(desc.description(), m_frame);
    label->setWordWrap(true);
    hlayout->addWidget(label);
    EffectWidget *w = new EffectWidget(&m_effect, m_frame);
    hlayout->addWidget(w);
}
