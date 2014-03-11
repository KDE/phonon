/*
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#include "phononglobal.h"

#include "objectdescription.h"

#include "factory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

namespace Phonon {

QString categoryToString(Category c)
{
    switch(c)
    {
    case Phonon::NoCategory:
        break;
    case Phonon::NotificationCategory:
        return QCoreApplication::translate("Phonon::", "Notifications");
    case Phonon::MusicCategory:
        return QCoreApplication::translate("Phonon::", "Music");
    case Phonon::VideoCategory:
        return QCoreApplication::translate("Phonon::", "Video");
    case Phonon::CommunicationCategory:
        return QCoreApplication::translate("Phonon::", "Communication");
    case Phonon::GameCategory:
        return QCoreApplication::translate("Phonon::", "Games");
    case Phonon::AccessibilityCategory:
        return QCoreApplication::translate("Phonon::", "Accessibility");
    }
    return QString();
}

QString categoryToString(CaptureCategory c)
{
    switch(c)
    {
    case Phonon::NoCategory:
        break;
    case Phonon::CommunicationCategory:
        return QCoreApplication::translate("Phonon::", "Communication");
    case Phonon::RecordingCaptureCategory:
        return QCoreApplication::translate("Phonon::", "Recording");
    case Phonon::ControlCaptureCategory:
        return QCoreApplication::translate("Phonon::", "Control");
    }
    return QString();
}

QDebug operator <<(QDebug dbg, const Phonon::MetaData &metaData)
{
    switch (metaData) {
    case Phonon::ArtistMetaData:
        dbg.space() << "Phonon::ArtistMetaData";
        break;
    case Phonon::AlbumMetaData:
        dbg.space() << "Phonon::AlbumMetaData";
        break;
    case Phonon::TitleMetaData:
        dbg.space() << "Phonon::TitleMetaData";
        break;
    case Phonon::DateMetaData:
        dbg.space() << "Phonon::DateMetaData";
        break;
    case Phonon::GenreMetaData:
        dbg.space() << "Phonon::GenreMetaData";
        break;
    case Phonon::TrackNumberMetaData:
        dbg.space() << "Phonon::TracknumberMetaData";
        break;
    case Phonon::DescriptionMetaData:
        dbg.space() << "Phonon::DescriptionMetaData";
        break;
    }
    return dbg.maybeSpace();
}

QDebug operator <<(QDebug dbg, const Phonon::State &state)
{
    switch (state) {
    case Phonon::StoppedState:
        dbg.space() << "Phonon::StoppedState";
        break;
    case Phonon::PlayingState:
        dbg.space() << "Phonon::PlayingState";
        break;
    case Phonon::PausedState:
        dbg.space() << "Phonon::PausedState";
        break;
    }
    return dbg.maybeSpace();
}

QDebug operator <<(QDebug dbg, const Phonon::Category &category)
{
    dbg.space() << categoryToString(category);
    return dbg.maybeSpace();
}

QDebug operator <<(QDebug dbg, const Phonon::ErrorType &errorType)
{
    switch (errorType) {
    case Phonon::NoError:
        dbg.space() << "Phonon::NoError";
        break;
    case Phonon::NormalError:
        dbg.space() << "Phonon::NormalError";
        break;
    case Phonon::FatalError:
        dbg.space() << "Phonon::FatalError";
        break;
    }
    return dbg.maybeSpace();
}

} // namespace Phonon

static int registerPhononMetaTypes()
{
    qRegisterMetaType<Phonon::State>();
    qRegisterMetaType<Phonon::ErrorType>();
    qRegisterMetaType<Phonon::Category>();
    qRegisterMetaType<Phonon::CaptureCategory>();

    // need those for QSettings
    qRegisterMetaType<QList<int> >();
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

#warning anything
    return 0; // something
}

#ifdef Q_CONSTRUCTOR_FUNCTION
Q_CONSTRUCTOR_FUNCTION(registerPhononMetaTypes)
#else
static const int _Phonon_registerMetaTypes = registerPhononMetaTypes();
#endif
