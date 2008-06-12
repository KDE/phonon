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

#include "titlewidget.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent), m_iface(0)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    QVBoxLayout *topLayout = new QVBoxLayout(this);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    topLayout->addLayout(hlayout);
    QLabel *l = new QLabel("current title:", this);
    m_currentTitle = new QSpinBox(this);
    hlayout->addWidget(l);
    hlayout->addWidget(m_currentTitle);
    l->setBuddy(m_currentTitle);
    m_currentTitle->setRange(1, 1);

    hlayout = new QHBoxLayout(this);
    topLayout->addLayout(hlayout);
    l = new QLabel("available titles:", this);
    m_availableTitles = new QLabel("0", this);
    hlayout->addWidget(l);
    hlayout->addWidget(m_availableTitles);

    hlayout = new QHBoxLayout(this);
    topLayout->addLayout(hlayout);
    l = new QLabel("autoplay:", this);
    m_autoplay = new QToolButton(this);
    m_autoplay->setCheckable(true);
    hlayout->addWidget(l);
    hlayout->addWidget(m_autoplay);
}

TitleWidget::~TitleWidget()
{
}

void TitleWidget::setInterface(MediaController *i)
{
    if (m_iface) {
        disconnect(m_iface, 0, m_currentTitle, 0);
        disconnect(m_currentTitle, 0, m_iface, 0);
        disconnect(m_iface, 0, m_availableTitles, 0);
        disconnect(m_autoplay, 0, m_iface, 0);
    }
    m_iface = i;
    if (m_iface) {
        const int a = m_iface->availableTitles();
        m_currentTitle->setMaximum(a);
        m_availableTitles->setNum(a);
        m_currentTitle->setValue(m_iface->currentTitle());
        m_autoplay->setChecked(m_iface->autoplayTitles());

        connect(m_iface, SIGNAL(availableTitlesChanged(int)), SLOT(availableTitlesChanged(int)));
        connect(m_iface, SIGNAL(availableTitlesChanged(int)), m_availableTitles, SLOT(setNum(int)));
        connect(m_iface, SIGNAL(titleChanged(int)), m_currentTitle, SLOT(setValue(int)));
        connect(m_currentTitle, SIGNAL(valueChanged(int)), m_iface, SLOT(setCurrentTitle(int)));
        connect(m_autoplay, SIGNAL(toggled(bool)), m_iface, SLOT(setAutoplayTitles(bool)));
    }
}

void TitleWidget::availableTitlesChanged(int x) { m_currentTitle->setMaximum(x); }

