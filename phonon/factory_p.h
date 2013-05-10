/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_FACTORY_P_H
#define PHONON_FACTORY_P_H

#include "phonon_export.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

class QUrl;
class QIcon;

namespace Phonon
{
    class PlatformPlugin;
    class FrontendPrivate;
    class AbstractMediaStream;

/**
 * \internal
 * \brief Factory to access the preferred Backend.
 *
 * This class is used internally to get the backend's implementation.
 * It keeps track of the objects that were created. When a
 * request for a backend change comes, it asks all frontend objects to delete
 * their backend objects and then checks whether they were all deleted. Only
 * then the old backend is unloaded and the new backend is loaded.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
namespace Factory
{
    /**
     * Emits signals for Phonon::Factory.
     */
    class Sender : public QObject
    {
        Q_OBJECT
        Q_SIGNALS:
            /**
             * \copydoc BackendCapabilities::Notifier::availableAudioOutputDevicesChanged
             */
            void availableAudioOutputDevicesChanged();

            /**
             * \copydoc BackendCapabilities::Notifier::availableAudioCaptureDevicesChanged
             */
            void availableAudioCaptureDevicesChanged();

            /**
             * \copydoc BackendCapabilities::Notifier::availableVideoCaptureDevicesChanged
             */
            void availableVideoCaptureDevicesChanged();
    };

    /**
     * Returns a pointer to the object emitting the signals.
     *
     * \see Sender::backendChanged()
     */
    PHONON_EXPORT Sender *sender();

#warning createObject!
    QObject *createPlayer(QObject *parent = 0);

    /**
     * Create a new backend object for a Effect.
     *
     * \return a pointer to the Effect the backend provides.
     */
#ifndef QT_NO_PHONON_EFFECT
    QObject *createEffect(int effectId, QObject *parent = 0);
#endif //QT_NO_PHONON_EFFECT
    /**
     * Create a new backend object for a VolumeFaderEffect.
     *
     * \return a pointer to the VolumeFaderEffect the backend provides.
     */
#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT
    QObject *createVolumeFaderEffect(QObject *parent = 0);
#endif //QT_NO_PHONON_VOLUMEFADEREFFECT
    /**
     * Create a new backend object for a AudioOutput.
     *
     * \return a pointer to the AudioOutput the backend provides.
     */
    QObject *createAudioOutput(QObject *parent = 0);
    /**
     * Create a new backend object for a VideoWidget.
     *
     * \return a pointer to the VideoWidget the backend provides.
     */
#ifndef QT_NO_PHONON_VIDEO
    QObject *createVideoWidget(QObject *parent = 0);
#endif //QT_NO_PHONON_VIDEO

    /**
    * Create a new backend object for a AudioDataOutput.
    *
    * \return a pointer to the AudioDataOutput the backend provides.
    */
    PHONON_EXPORT QObject *createAudioDataOutput(QObject *parent = 0);

    /**
     * \return a pointer to the backend interface.
     */
    PHONON_EXPORT QObject *backend(bool createWhenNull = true);

    /**
     * Unique identifier for the Backend. Can be used in configuration files
     * for example.
     */
    QString identifier();

    /**
     * Get the name of the Backend. It's the name from the .desktop file.
     */
    PHONON_EXPORT QString backendName();

    /**
     * Get the comment of the Backend. It's the comment from the .desktop file.
     */
    QString backendComment();

    /**
     * Get the version of the Backend. It's the version from the .desktop file.
     *
     * The version is especially interesting if there are several versions
     * available for binary incompatible versions of the backend's media
     * framework.
     */
    QString backendVersion();

    /**
     * Get the icon (name) of the Backend. It's the icon from the .desktop file.
     */
    QString backendIcon();

    /**
     * Get the website of the Backend. It's the website from the .desktop file.
     */
    QString backendWebsite();

    /**
     * registers the backend object
     */
    PHONON_EXPORT QObject *registerQObject(QObject *o);

    PHONON_EXPORT void registerFrontendObject(FrontendPrivate *);
    PHONON_EXPORT void deregisterFrontendObject(FrontendPrivate *);

    PHONON_EXPORT void setBackend(QObject *);

    PHONON_EXPORT PlatformPlugin *platformPlugin();

} // namespace Factory
} // namespace Phonon

#endif // PHONON_FACTORY_P_H
