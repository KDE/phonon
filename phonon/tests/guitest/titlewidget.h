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

#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <phonon/mediacontroller.h>

using Phonon::MediaController;

class TitleWidget : public QWidget
{
    Q_OBJECT
    public:
        TitleWidget(QWidget *parent = 0);
        ~TitleWidget();
        void setInterface(MediaController *i);

    private slots:
        void availableTitlesChanged(int);
    private:
        MediaController *m_iface;
        QSpinBox *m_currentTitle;
        QLabel *m_availableTitles;
        QToolButton *m_autoplay;
};

#endif // TITLEWIDGET_H
