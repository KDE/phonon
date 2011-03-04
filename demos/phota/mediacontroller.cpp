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

#include <QtGui/QFileDialog>
//#include <Phonon/MediaSource>
//#include <Phonon/MediaObject>
#include <phonon/mediasource.h>
#include <phonon/mediaobject.h>

using namespace Phonon;

MediaController::MediaController(QObject *parent) :
    QObject(parent)
{
    media = new MediaObject(this);
}

void MediaController::openFile()
{
    QString file = QFileDialog::getOpenFileName( NULL, "Open a new file to play", "" );
    if( !file.isEmpty() )
    {
        MediaSource s(file);
        playSource( s );
    }
}

void MediaController::openURL()
{

}

void MediaController::playSource( const MediaSource &s )
{
    media->setCurrentSource(s);
    media->play();
}
