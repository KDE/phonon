/*
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_MEDIACONTROLLER_H
#define PHONON_MEDIACONTROLLER_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QObject>
#include <QtCore/QtGlobal>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
class MediaControllerPrivate;
class MediaObject;

/** \class MediaController mediacontroller.h phonon/MediaController
 * \brief Controls optional features of a media file/device like title, chapter, angle.
 *
 * \ingroup Playback
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT MediaController : public QObject
{
    Q_OBJECT
    Q_FLAGS(Features)
    public:
        enum Feature {
            /**
             * In the VOB (DVD) format, it is possible to give several video streams
             * of the same scene, each of which displays the scene from a different
             * angle. The DVD viewer can then change between these angles.
             */
            Angles = 1,
            /**
             * In the VOB format, chapters are points in a single video stream
             * that can be played and seeked to separately.
             */
            Chapters = 2,
            /**
             * In the VOB format, navigations are menus to quickly navigate
             * to content.
             */
            Navigations = 3,
            /**
             * On a CD, a title is a separate sound track. On DVD, a title is a
             * separate VOB file (i.e. usually a different video entity).
             */
            Titles = 4
        };
        Q_DECLARE_FLAGS(Features, Feature)

        enum NavigationMenu {
            RootMenu,     /** < Root/main menu. */
            TitleMenu,    /** < Title Menu to access different titles on the media source.
                                The title menu is usually where one would select
                                the episode of a TV series DVD. It can be equal to
                                the main menu but does not need to be. */
            AudioMenu,    /** < Audio menu for language (and somtimes also subtitle)
                                settings etc. */
            SubtitleMenu, /** < Subtitle menu. Usually this represents the same menu
                                as AudioMenu or is not present at all (in which case
                                subtitle settings are propably also in the AudioMenu). */
            ChapterMenu,  /** < Chapter menu for chapter selection. */
            AngleMenu     /** < Angle menu. Rarely supported on any media source. */
        };

        MediaController(MediaObject *parent);
        ~MediaController();

        Features supportedFeatures() const;

        int availableAngles() const;
        int currentAngle() const;

        int availableChapters() const;
        int currentChapter() const;

        /**
         * Translates a NavigationMenu enum to a string you can use in your GUI.
         * Please note that keyboard shortucts will not be present in the returned
         * String, therefore it is probably not a good idea to use this function
         * if you are providing keyboard shortcuts for every other clickable.
         *
         * Please note that RootMenu has the string representation "Main Menu" as
         * root is a rather technical term when talking about menus.
         *
         * Example:
         * \code
         * QString s = Phonon::MediaController::navigationMenuToString(MenuMain);
         * // s now contains "Main Menu"
         * \endcode
         *
         * \returns the QString representation of the menu
         */
        static QString navigationMenuToString(NavigationMenu menu);

        /**
         * Get the list of currently available menus for the present media source.
         *
         * The list is always ordered by occurance in the NavgiationMenu enum.
         * Should you wish to use a different order in your application you will
         * have to make appropriate changes.
         *
         * \returns list of available menus (supported by backend and media source).
         *
         * \see navigationMenuToString()
         */
        QList<NavigationMenu> availableMenus() const;

        int availableTitles() const;
        int currentTitle() const;

        bool autoplayTitles() const;

        /**
         * Returns the selected audio stream.
         *
         * \see availableAudioChannels
         * \see setCurrentAudioChannel
         */
        AudioChannelDescription currentAudioChannel() const;

        /**
         * Returns the selected subtitle stream.
         *
         * \see availableSubtitles
         * \see setCurrentSubtitle
         */
        SubtitleDescription currentSubtitle() const;

        /**
         * Returns the audio streams that can be selected by the user. The
         * strings can directly be used in the user interface.
         *
         * \see selectedAudioChannel
         * \see setCurrentAudioChannel
         */
        QList<Phonon::AudioChannelDescription> availableAudioChannels() const;

        /**
         * Returns the subtitle streams that can be selected by the user. The
         * strings can directly be used in the user interface.
         *
         * \see selectedSubtitle
         * \see setCurrentSubtitle
         */
        QList<SubtitleDescription> availableSubtitles() const;

        /**
         * Selects an audio stream from the media.
         *
         * Some media formats allow multiple audio streams to be stored in
         * the same file. Normally only one should be played back.
         *
         * \param stream Description of an audio stream
         *
         * \see availableAudioChannels()
         * \see currentAudioChannel()
         */
        void setCurrentAudioChannel(const Phonon::AudioChannelDescription &stream);

        /**
         * Switches to a menu (e.g. on a DVD).
         *
         * \see availableMenus()
         */
        void setCurrentMenu(NavigationMenu menu);

        /**
         * Selects a subtitle stream from the media.
         *
         * Some media formats allow multiple subtitle streams to be stored in
         * the same file. Normally only one should be displayed.
         *
         * \param stream description of a subtitle stream
         *
         * \see availableSubtitles()
         * \see currentSubtitle()
         */
        void setCurrentSubtitle(const Phonon::SubtitleDescription &stream);

    public Q_SLOTS:
        void setCurrentAngle(int angleNumber);
        void setCurrentChapter(int chapterNumber);

        /**
         * Skips to the given title \p titleNumber.
         *
         * If it was playing before the title change it will start playback on the new title if
         * autoplayTitles is enabled.
         */
        void setCurrentTitle(int titleNumber);
        void setAutoplayTitles(bool);

        /**
         * Skips to the next title.
         *
         * If it was playing before the title change it will start playback on the next title if
         * autoplayTitles is enabled.
         */
        void nextTitle();

        /**
         * Skips to the previous title.
         *
         * If it was playing before the title change it will start playback on the previous title if
         * autoplayTitles is enabled.
         */
        void previousTitle();

    Q_SIGNALS:
        void availableAnglesChanged(int availableAngles);
        void availableAudioChannelsChanged();
        void availableChaptersChanged(int availableChapters);

        /**
         * The available menus changed, this for example emitted when Phonon switches
         * from a media source without menus to one with menus (e.g. a DVD).
         *
         * \param menus is a list of all currently available menus, you should update
         * GUI representations of the available menus with the new set.
         *
         * \see availableMenus()
         * \see navigationMenuToString()
         */
        void availableMenusChanged(QList<NavigationMenu> menus);
        void availableSubtitlesChanged();
        void availableTitlesChanged(int availableTitles);

        void angleChanged(int angleNumber);
        void chapterChanged(int chapterNumber);
        void titleChanged(int titleNumber);

    protected:
        MediaControllerPrivate *const d;
};

} // namespace Phonon

Q_DECLARE_OPERATORS_FOR_FLAGS(Phonon::MediaController::Features)

Q_DECLARE_METATYPE(Phonon::MediaController::NavigationMenu)
Q_DECLARE_METATYPE(QList<Phonon::MediaController::NavigationMenu>)

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_MEDIACONTROLLER_H
