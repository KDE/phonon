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

#ifndef WIDGETRECTITEM_H
#define WIDGETRECTITEM_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QPoint>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsView>

class WidgetRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    protected:
        WidgetRectItem(QGraphicsItem *parent, QGraphicsView *widget);
        WidgetRectItem(const QPoint &pos, QGraphicsView *widget);

    public:
        ~WidgetRectItem();

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        enum { Type = UserType + 1 };
        int type() const { return Type; }

        QObject *qObject() { return this; }
        const QObject *qObject() const { return this; }

        bool eventFilter(QObject *obj, QEvent *e);

    signals:
        void itemMoved(const QRectF &newSceneCenterPos);

    protected:
        void setTitle(const QString &title);

        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        QWidget *m_frame;

    private:
        QGraphicsView *m_view;
        QString m_title;
};

#endif // WIDGETRECTITEM_H
