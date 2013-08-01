/*  This file is part of the KDE project
    Copyright (C) 2011 Jakub Spiewak <jmspiewak@gmail.com>

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

#include "metadatareader.h"

#include <phonon/MediaObject>


MetaDataReader::MetaDataReader(QString &file, QTextStream &out):
    textStream(out)
{
    mediaObj = new Phonon::MediaObject(this);

    connect(mediaObj, SIGNAL(metaDataChanged()), this, SLOT(printMetaData()));
    connect(mediaObj, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(checkForError(Phonon::State,Phonon::State)));

    mediaObj->setCurrentSource(QUrl::fromLocalFile(file));
}


MetaDataReader::~MetaDataReader()
{
}


void MetaDataReader::printMetaData()
{
    textStream << "Meta data of file " << mediaObj->currentSource().fileName() << ":\n";

    typedef QMultiMap<QString, QString> MetaDataMap;
    MetaDataMap metaData = mediaObj->metaData();

    for(MetaDataMap::iterator it=metaData.begin(); it!=metaData.end(); ++it)
    {
        textStream << it.key() << ": " << it.value() << '\n';
    }

    textStream.flush();
    quit();
}


void MetaDataReader::checkForError(Phonon::State state, Phonon::State)
{
    if(state == Phonon::ErrorState)
    {
        textStream << "Error: unable to read meta data of " << mediaObj->currentSource().fileName() << endl;
        quit();
    }
}
