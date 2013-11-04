/*  This file is part of the KDE project
    Copyright (C) 2007-2008 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_ADDONINTERFACE_H
#define PHONON_ADDONINTERFACE_H

#include "phononnamespace.h"

#include <QtCore/QList>
#include <QtCore/QVariant>


#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
/** \class AddonInterface addoninterface.h phonon/AddonInterface
 * \short Interface for Menu, Chapter, Angle and Title/Track control.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class AddonInterface
{
    public:
        virtual ~AddonInterface() {}

        enum Interface {
            NavigationInterface   = 1, /**< Interface for (menu) navigation */
            ChapterInterface      = 2, /**< Interface for chapter control */
            AngleInterface        = 3, /**< Interface for angle control */
            TitleInterface        = 4, /**< Interface for title control */
            SubtitleInterface     = 5, /**< Interface for subtitle control */
            AudioChannelInterface = 6  /**< Interface for audio channel control */
        };

        enum NavigationCommand {
            availableMenus, /**< \returns a QList<MediaController::NavigationMenu>
                                 containing all supported navigation menu types */
            setMenu         /**< Sets the current menu to the first
                                 \c MediaController::NavigationMenu in a QList */
        };

        enum ChapterCommand {
            availableChapters, /**< \returns an \c int representing the amount of
                                    available chapters on the media source */
            chapter,           /**< \returns an \c int representing the current chapter */
            setChapter         /**< Sets the current chapter to the first \c int in the QList */
        };

        enum AngleCommand {
            availableAngles, /**< \returns \c int representing the amount of
                                  available angles on the media source */
            angle,           /**< \returns an \c int representing the current angle */
            setAngle         /**< Sets the current angle to the first \c int in the QList */
        };

        enum TitleCommand {
            availableTitles, /**< \returns \c int representing the amount of
                                  available titles on the media source */
            title,           /**< \returns \c int representing the current title */
            setTitle,        /**< Sets the current tittle to the first \c int in the QList */
            autoplayTitles,  /**< \returns \c bool whether autoplay of titles is on */
            setAutoplayTitles /**< Sets autoplay to \c true or \c false as
                                   indicated in the first \c bool in the QList */
        };

        enum SubtitleCommand {
            availableSubtitles, /**< \returns \c int representing the amount of
                                      available subtitles on the media source */
            currentSubtitle,    /**< \returns \c int representing the current subtitle */
            setCurrentSubtitle, /**< Sets the current subtitle to the first
                                     \c int in the QList */
            setCurrentSubtitleFile, /**< Sets the current subtitle to the first QUrl \since 4.7.0 */
            subtitleAutodetect, /**< \returns \c bool representing if subtitles
                                      autodetection is enabled \since 4.7.0 */
            setSubtitleAutodetect, /**< Sets/Unsets subtitles autodetection \since 4.7.0 */
            subtitleEncoding, /**< \returns a QString representing the current encoding
                                   used to render subtitles \since 4.7.0 */
            setSubtitleEncoding, /** Sets the current encoding used to render subtitles \since 4.7.0 */
            subtitleFont, /**< \returns a QFont representing the current font used
                              to render subtitles \since 4.7.0 */
            setSubtitleFont /**< Sets the current font used to render subtitles \since 4.7.0 */
        };

        enum AudioChannelCommand {
            availableAudioChannels, /**< \returns \c int representing the amount
                                          of all available audio channels on the
                                          media source */
            currentAudioChannel,    /**< \returns \c int representing the current
                                          audio channel */
            setCurrentAudioChannel  /**< Sets the current audio channel to the first
                                         \c int in the QList */
        };

        /**
         * Queries whether the backend supports a specific interface.
         *
         * \param iface The interface to query support information about
         * \returns \c true when the backend supports the interface, \c false otherwise
         * 
         * \ingroup backend
         **/
        virtual bool hasInterface(Interface iface) const = 0;

        /**
         * Calls an interface on the backend.
         *
         * \param iface The interface to call.
         * \param command The command the interface shall execute. This can be
         * any value of the Command enumeration associated with the command. The
         * backend casts this appropriately.
         * \param arguments The arguments for the command. This list can contain
         * a QVariant supported format + additions specific to Phonon. The
         * content entirely depends on the command (e.g. a getter may simply use
         * an empty list).
         * 
         * \return \c QVariant, as with the arguments this can be anything ranging
         * from an empty QVariant to custom types used within Phonon
         * 
         * Setting the chapter of a Media could be done like this: 
         * \code
         * AddonInterface *iface = d->iface();
         * iface->interfaceCall(AddonInterface::ChapterInterface,
         *                      AddonInterface::setChapter,
         *                      QList<QVariant>() << QVariant(titleNumber));
         * \endcode
         * 
         * Handling such a request in the backend is done as follows:
         * \code
         * switch (iface) {
         * case AddonInterface::ChapterInterface:
         *     switch (static_cast<AddonInterface::ChapterCommand>(command)) {
         *     case setChapter:
         *         setCurrentChapter(arguments.first().toInt());
         *         return QVariant();
         *     }
         * }
         * \endcode
         * 
         * \see Interface
         * \see NavigationCommand
         * \see ChapterCommand
         * \see AngleCommand
         * \see TitleCommand
         * \see SubtitleCommand
         * \see AudioChannelCommand
         * 
         * \ingroup backend
         **/
        virtual QVariant interfaceCall(Interface iface, int command,
                const QList<QVariant> &arguments = QList<QVariant>()) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::AddonInterface, "AddonInterface0.2.phonon.kde.org")

#endif //QT_NO_PHONON_MEDIACONTROLLER


#endif // PHONON_ADDONINTERFACE_H
