/*
    Copyright (C) 2005-2008 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#ifndef PHONONNAMESPACE_H
#define PHONONNAMESPACE_H

#include "phonon_export.h"

class QString;

namespace Phonon
{
enum DiscType {
    NoDisc = -1, /** < No disc was selected. Only used as default value in
                          \class MediaSource */
    Cd = 0,      /** < Audio CD */
    Dvd = 1,     /** < Video DVD (no arbitrary data DVDs) */
    Vcd = 2,     /** < Video CD */
    BluRay = 3   /** < BluRay video disc \since 4.7.0 */
};

// http://xiph.org/vorbis/doc/v-comment.html
enum MetaData {
    ArtistMetaData,
    AlbumMetaData,
    TitleMetaData,
    DateMetaData,
    GenreMetaData,
#warning TrackNumber or Tracknumber
    TracknumberMetaData,
    DescriptionMetaData,
    CopyrightMetaData,
    LicenseMetaData,
    OrganizationMetaData,
    DateMetaData,
    LocationMetaData,
    ContactMetaData,
    IsrcMetaData,
#warning this here field is ENOSTANDARD, what to do with it
    MusicBrainzDiscIdMetaData
#warning VLC also has additional data though some of those may be video or something, needs investigation
//    UrlMetaData, supposedly the media URL?
//    RatingMetaData,
//    SettingMetaData, may be location from above
//    LanguageMetaData, may be video
//    NowPlayingMetaData, probably bool playing or possible a nowplaying of a stream?
//    PublisherMetaData, probably Organization from above
//    EncodedByMetaData,
//    ArtworkUrlMetaData, sounds vlc specific (e.g. downloaded album cover)
};

enum State {
    StoppedState,
    PlayingState,
    BufferingState,
    PausedState,
    ErrorState
};

enum Category {
    NoCategory = -1,
    NotificationCategory = 0,
    MusicCategory = 1,
    VideoCategory = 2,
    CommunicationCategory = 3,
    GameCategory = 4,
    AccessibilityCategory = 5,
    LastCategory = AccessibilityCategory
};

enum CaptureCategory {
    NoCaptureCategory = NoCategory,
    CommunicationCaptureCategory = CommunicationCategory,
    RecordingCaptureCategory,
    ControlCaptureCategory = AccessibilityCategory
};

#warning why namespace?
namespace Capture {
enum DeviceType {
    AudioType,
    VideoType
};
} // namespace Capture

enum ErrorType {
    NoError = 0,
    NormalError = 1,
    FatalError = 2
};

PHONON_EXPORT QString categoryToString(Category c);
PHONON_EXPORT QString categoryToString(CaptureCategory c);

PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::DiscType &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::MetaData &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::State &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::Category &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::CaptureCategory &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::Capture::DeviceType &);
PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::ErrorType &);

} // namespace Phonon

#include <QtCore/QMetaType>

Q_DECLARE_METATYPE(Phonon::State)
Q_DECLARE_METATYPE(Phonon::ErrorType)
Q_DECLARE_METATYPE(Phonon::Category)
Q_DECLARE_METATYPE(Phonon::CaptureCategory)

#endif // PHONONNAMESPACE_H
