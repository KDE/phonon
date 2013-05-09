/*
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_PLATFORMPLUGIN_H
#define PHONON_PLATFORMPLUGIN_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QStringList>

class QUrl;
class QObject;
class QIcon;

namespace Phonon {

class AbstractMediaStream;

class PlatformPlugin
{
public:
    virtual ~PlatformPlugin() {}

    /**
     * Creates a AbstractMediaStream object that provides the data for the given \p url. On KDE
     * this uses KIO.
     */
    virtual AbstractMediaStream *createMediaStream(const QUrl &url, QObject *parent) = 0;

    /**
     * Returns the icon for the given icon name.
     */
    virtual QIcon icon(const QString &name) const = 0;

    /**
     * Shows a notification popup
     */
    virtual void notification(const char *notificationName, const QString &text,
                              const QStringList &actions = QStringList(), QObject *receiver = 0,
                              const char *actionSlot = 0) const = 0;

    /**
     * Returns the name of the application. For most Qt application this is
     * QCoreApplication::applicationName(), but for KDE this is overridden by KAboutData.
     */
    virtual QString applicationName() const = 0;

    /**
     * Creates a backend object. This way the platform can decide the backend preference.
     */
    virtual QObject *createBackend() = 0;

    /**
     * Using the library loader of the platform, loads a given backend.
     */
    virtual QObject *createBackend(const QString &library, const QString &version) = 0;

    virtual QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const = 0;
    virtual QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const = 0;

    /**
     * Returns a list of (driver, handle) pairs for the given AudioOutputDevice description.
     * Implementation is optional.
     */
    virtual DeviceAccessList deviceAccessListFor(const AudioOutputDevice &) const { return DeviceAccessList(); }

#ifndef PHONON_NO_AUDIOCAPTURE
    /**
     * Returns a list of (driver, handle) pairs for the given AudioCaptureDevice description.
     * Implementation is optional.
     */
    virtual DeviceAccessList deviceAccessListFor(const AudioCaptureDevice &) const { return DeviceAccessList(); }
#endif // PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
    /**
     * Returns a list of (driver, handle) pairs for the given VideoCaptureDevice description.
     * Implementation is optional.
     */
    virtual DeviceAccessList deviceAccessListFor(const VideoCaptureDevice &) const { return DeviceAccessList(); }
#endif // PHONON_NO_VIDEOCAPTURE
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::PlatformPlugin, "org.kde.phonon.PlatformPlugin/5.0")

#endif // PHONON_PLATFORMPLUGIN_H
