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

#include "mediacontroller.h"

#include <phonon/mediasource.h>
#include <phonon/mediaobject.h>

#include <QFileDialog>

MediaController::MediaController(QObject *parent) :
    QObject(parent)
{
    m_media = new Phonon::MediaObject(this);
}

Phonon::MediaObject *MediaController::mediaObject() const
{
    return m_media;
}

void MediaController::openFile()
{
    QUrl file = QUrl::fromLocalFile(QFileDialog::getOpenFileName(0, tr("Open a new file to play"), ""));
    if (!file.isEmpty()) {
        Phonon::MediaSource s(file);
        playSource(s);
    }
}

void MediaController::openURL()
{

}

void MediaController::playSource(const Phonon::MediaSource &s)
{
    m_media->setCurrentSource(s);
    m_media->play();
}
