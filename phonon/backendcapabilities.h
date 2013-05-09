/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_BACKENDCAPABILITIES_H
#define Phonon_BACKENDCAPABILITIES_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QObject>


#ifdef __QT_SYNCQT__
// Tell syncqt that the BackendCapabilities namespace should be treated like a class
#pragma qt_class(Phonon::BackendCapabilities)
#pragma qt_sync_stop_processing
#endif

template<class T> class QList;
class QStringList;

namespace Phonon
{

/**
 * Collection of functions describing the capabilities of the Backend.
 *
 * \ingroup BackendInformation
 * \author Matthias Kretz <kretz@kde.org>
 */
namespace BackendCapabilities
{
    /** \class Notifier backendcapabilities.h phonon/BackendCapabilities
     * Notifications about backend capabilities.
     *
     * \ingroup BackendInformation
     */
    class Notifier : public QObject
    {
        Q_OBJECT
        Q_SIGNALS:
            /**
             * This signal is emitted if the capabilities have changed. This can
             * happen if the user has requested a backend change.
             */
            void capabilitiesChanged();

            /**
             * This signal is emitted when audio output devices were plugged or
             * unplugged.
             *
             * Check BackendCapabilities::availableAudioOutputDevices to get the
             * current list of available devices.
             */
            void availableAudioOutputDevicesChanged();

#ifndef PHONON_NO_AUDIOCAPTURE
            /**
             * This signal is emitted when audio capture devices were plugged or
             * unplugged.
             *
             * Check BackendCapabilities::availableAudioCaptureDevices to get the
             * current list of available devices.
             */
            void availableAudioCaptureDevicesChanged();
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
            /**
             * This signal is emitted when video capture devices were plugged or
             * unplugged.
             *
             * Check BackendCapabilities::availableVideoCaptureDevices to get the
             * current list of available devices.
             */
            void availableVideoCaptureDevicesChanged();
#endif //PHONON_NO_VIDEOCAPTURE
    };

    /**
     * Use this function to get a QObject pointer to connect to one of the Notifier signals.
     *
     * \return a pointer to a QObject.
     *
     * To connect to the signal do the following:
     * \code
     * QObject::connect(BackendCapabilities::notifier(), SIGNAL(capabilitiesChanged()), ...
     * \endcode
     *
     * \see Notifier::capabilitiesChanged()
     * \see Notifier::availableAudioOutputDevicesChanged()
     * \see Notifier::availableAudioCaptureDevicesChanged()
     * \see Notifier::availableVideoCaptureDevicesChanged()
     */
    PHONON_EXPORT Notifier *notifier();

    /**
     * Returns the audio output devices the backend supports.
     *
     * \return A list of AudioOutputDevice objects that give a name and
     * description for every supported audio output device.
     */
    PHONON_EXPORT QList<AudioOutputDevice> availableAudioOutputDevices();

#ifndef PHONON_NO_AUDIOCAPTURE
    /**
     * Returns the audio capture devices the backend supports.
     *
     * \return A list of AudioCaptureDevice objects that give a name and
     * description for every supported audio capture device.
     */
    PHONON_EXPORT QList<AudioCaptureDevice> availableAudioCaptureDevices();
#endif //PHONON_NO_AUDIOCAPTURE

    /**
     * Returns the video output devices the backend supports.
     *
     * \return A list of VideoOutputDevice objects that give a name and
     * description for every supported video output device.
     */
//    PHONON_EXPORT QList<VideoOutputDevice> availableVideoOutputDevices();

#ifndef PHONON_NO_VIDEOCAPTURE
    /**
     * Returns the video capture devices the backend supports.
     *
     * \return A list of VideoCaptureDevice objects that give a name and
     * description for every supported video capture device.
     */
    PHONON_EXPORT QList<VideoCaptureDevice> availableVideoCaptureDevices();
#endif //PHONON_NO_VIDEOCAPTURE

    /**
     * Returns the video capture devices that have audio capture capabilities
     * that the backend supports. In effect, these are both video and audio
     * capture devices and one can connect them to both a VideoWidget and an
     * AudioOutput, for example.
     *
     * The resulting VideoCaptureDevices have a "hasaudio" property to true.
     *
     * \note These devices appear both in availableVideoCaptureDevices() and
     * availableAudioCaptureDevices()
     *
     * \warning Creating two separate MediaObject instances for the same capture
     * device, one for video and the other for audio, most probably doesn't work.
     * But, if there are two separate devices, use Experimental::AVCapture.
     *
     * \see availableVideoCaptureDevices()
     * \see availableAudioCaptureDevices()
     * \see Experimental::AVCapture
     */
#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
    PHONON_EXPORT QList<VideoCaptureDevice> availableAVCaptureDevices();
#endif // NOT PHONON_NO_VIDEOCAPTURE AND NOT PHONON_NO_AUDIOCAPTURE

} // namespace BackendCapabilities
} // namespace Phonon


#endif // Phonon_BACKENDCAPABILITIES_H
// vim: sw=4 ts=4 tw=80
