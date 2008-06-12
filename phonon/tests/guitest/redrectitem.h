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

#ifndef REDRECTITEM_H
#define REDRECTITEM_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <QtGui/QGraphicsRectItem>

class RedRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    public:
        RedRectItem(const QPointF &pos)
            : m_pos(pos)
        {
            const QPointF offset(12.0, 12.0);
            setRect(QRectF(pos - offset, pos + offset));
            setPen(Qt::NoPen);
            setBrush(QColor(255, 0, 0, 80));
            setZValue(1.0);
            QTimer::singleShot(100, this, SLOT(expand()));
        }

    private slots:
        void expand()
        {
            const QPointF offset(25.0, 25.0);
            setRect(QRectF(m_pos - offset, m_pos + offset));
            QTimer::singleShot(100, this, SLOT(deleteLater()));
        }

    private:
        const QPointF m_pos;
};

#endif // REDRECTITEM_H
