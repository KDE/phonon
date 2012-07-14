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

/**
 * Meta data container for Qt Quick.
 *
 * Since meta data are rather difficult to access properly from within QML this
 * container allows simplified access to all the metadata of a MediaObject.
 * The container cannot be created from within QML but only obtained from
 * a MediaElement. The container internally does nothing more than access the
 * metadata getters of an associated MediaObject (this is precisely the reason
 * that one can only get an instance from a MediaElement - otherwise it would
 * not have a MediaObject to access).
 *
 * A MetaData instance stays valid for exactly the same time as the MediaElement
 * from which it was obtained. Throughout this time frame you do not need to
 * request a new MetaData instance, as the MediaElement will always only return
 * one instance. This consequently also means that you can directly access the
 * metadata object from the MediaElement without storing it as the overhead is
 * almost not existing.
 *
 * \author Harald Sitter <sitter@kde.org>
 */
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
    /// emitted when any meta data changed.
    void metaDataChanged();

private:
    /// The MediaObject that is used for internal query of metadata
    MediaObject *m_mediaObject;
};

#undef P_PROPERTY_GETTER

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_METADATA_H
