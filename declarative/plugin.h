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

#ifndef PHONON_DECLARATIVE_PLUGIN_H
#define PHONON_DECLARATIVE_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

namespace Phonon
{

/**
 * \brief The Phonon QtDeclarative Elements.
 *
 * QtDeclarative, also known as Qt Quick and QML, enables declarative UI creation.
 * The Phonon::Declarative namespace contains elements that enable access to
 * multimedia features from within QML code by simply importing the Phonon
 * Qt Quick plugin.
 *
 * \code
 * import Phonon 1.0
 * \endcode
 *
 * A simple application may look like this
 * \code
 * import QtQuick 1.0
 * import Phonon 1.0
 * Item {
 *     Media {
 *         anchors.fill: parent
 *         source: "~/coolvideo.webm"
 *         AudioOutput { volume: 100 }
 *         VideoOutput { anchors.fill: parent }
 *     }
 * }
 * \endcode
 *
 * The Phonon Qt Quick plugin contains most of Phonon C++'s objects such as:
 * \list
 * \li MediaElement (MediaObject)
 * \li AudioOutputElement (AudioOutput)
 * \li VideoElement (VideoGraphicsObject)
 * \li SubtitleElement (MediaController - subtitle features)
 * \li VolumeFaderEffect (VolumeFaderEffect)
 * \endlist
 *
 * \author Harald Sitter <sitter@kde.org>
 */
namespace Declarative
{

class Plugin : public QDeclarativeExtensionPlugin
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "org.kde.phonon.DeclarativePlugin")
#endif
public:
    virtual void registerTypes(const char *uri);
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_PLUGIN_H
