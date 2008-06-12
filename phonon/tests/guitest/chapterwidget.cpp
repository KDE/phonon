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

#include "chapterwidget.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

ChapterWidget::ChapterWidget(QWidget *parent)
    : QWidget(parent), m_iface(0)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    QVBoxLayout *topLayout = new QVBoxLayout(this);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    topLayout->addLayout(hlayout);
    QLabel *l = new QLabel("current chapter:", this);
    m_currentChapter = new QSpinBox(this);
    hlayout->addWidget(l);
    hlayout->addWidget(m_currentChapter);
    l->setBuddy(m_currentChapter);
    m_currentChapter->setRange(1, 1);

    hlayout = new QHBoxLayout(this);
    topLayout->addLayout(hlayout);
    l = new QLabel("available chapters:", this);
    m_availableChapters = new QLabel("0", this);
    hlayout->addWidget(l);
    hlayout->addWidget(m_availableChapters);
}

ChapterWidget::~ChapterWidget()
{
}

void ChapterWidget::setInterface(MediaController *i)
{
    if (m_iface) {
        disconnect(m_iface, 0, m_currentChapter, 0);
        disconnect(m_currentChapter, 0, m_iface, 0);
        disconnect(m_iface, 0, m_availableChapters, 0);
    }
    m_iface = i;
    if (m_iface) {
        connect(m_iface, SIGNAL(availableChaptersChanged(int)), SLOT(availableChaptersChanged(int)));
        connect(m_iface, SIGNAL(availableChaptersChanged(int)), m_availableChapters, SLOT(setNum(int)));
        connect(m_iface, SIGNAL(chapterChanged(int)), m_currentChapter, SLOT(setValue(int)));
        connect(m_currentChapter, SIGNAL(valueChanged(int)), m_iface, SLOT(setCurrentChapter(int)));
    }
}

void ChapterWidget::availableChaptersChanged(int x) { m_currentChapter->setMaximum(x); }

