/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_DECLARATIVE_METADATA_H
#define PHONON_DECLARATIVE_METADATA_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "phonon/mediaobject.h"

namespace Phonon {
namespace Declarative {

/**
 * Macro to help with creating a property getter using the Phonon::MetaData enum.
 */
#define P_PROPERTY_GETTER(__cName) \
    public: QStringList __cName() const { return m_mediaObject->metaData(Phonon::__cName##MetaData ); } private:

class MetaData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList artist READ Artist NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList album READ Album NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList title READ Title NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList date READ Date NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList genre READ Genre NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList trackNumber READ Tracknumber NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList description READ Description NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList musicBrainzDiscId READ MusicBrainzDiscId NOTIFY metaDataChanged)
    P_PROPERTY_GETTER(Artist)
    P_PROPERTY_GETTER(Album)
    P_PROPERTY_GETTER(Title)
    P_PROPERTY_GETTER(Date)
    P_PROPERTY_GETTER(Genre)
    P_PROPERTY_GETTER(Tracknumber)
    P_PROPERTY_GETTER(Description)
    P_PROPERTY_GETTER(MusicBrainzDiscId)
public:
    MetaData(MediaObject *mediaObject, QObject *parent = 0);
    ~MetaData();

signals:
    void metaDataChanged();

private:
    MediaObject *m_mediaObject;
};

#undef P_PROPERTY_GETTER

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_METADATA_H
