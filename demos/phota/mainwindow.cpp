/*
    Copyright (C) 2010 Jean-Baptiste Kempf <jb@videolan.org>

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

#include "mainwindow.h"
#include "mediacontroller.h"
#include "videoeffects.h"

#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/VideoWidget>

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_videoWidget = new Phonon::VideoWidget();
    setCentralWidget(m_videoWidget);
    m_mediaControl = new MediaController(this);
    Phonon::createPath(m_mediaControl->mediaObject(), m_videoWidget);
    resize(600, 400);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(tr("Control Menu"), this);
    menu.addAction(tr("Open a file"), m_mediaControl, SLOT(openFile()));
    menu.addAction(tr("Open a URL"), m_mediaControl, SLOT(openURL()));
    menu.addSeparator();
    menu.addAction(tr("Video Effects"), this, SLOT(effectsDialog()));
    menu.exec(e->globalPos());
}

void MainWindow::effectsDialog()
{
    VideoEffects *effect = new VideoEffects(m_videoWidget, this);
    effect->show();
}
