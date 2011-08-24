/*  This file is part of the KDE project
    Copyright (C) 2005-2006, 2008 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_EXPERIMENTAL_AVCAPTURE_H
#define PHONON_EXPERIMENTAL_AVCAPTURE_H

#if defined(PHONON_NO_VIDEOCAPTURE) || defined(PHONON_NO_AUDIOCAPTURE)
#define NO_PHONON_AVCAPTURE
#endif

#ifndef NO_PHONON_AVCAPTURE

#include "export.h"
#include "../medianode.h"
#include "../phonondefs.h"
#include "objectdescription.h"

#if defined(MAKE_PHONONEXPERIMENTAL_LIB)
#include "../phononnamespace.h"
#else
#include "phonon/phononnamespace.h"
#endif

class QString;
class QStringList;

namespace Phonon
{
namespace Experimental
{

    class AvCapturePrivate;

    /**
     * @short Media data from a soundcard, soundserver, camera or any other
     * hardware device supported by the backend.
     *
     * This class gives you access to the capture capabilities of the backend.
     * There might be more than only one possible capture source, for audio, as
     * well as for video. A prefferable device may easily be obtained by providing
     * a capture category.
     *
     * @ingroup Recording
     * @author Matthias Kretz <kretz@kde.org>
     * @see BackendCapabilities::availableAudioCaptureDevices
     * @see BackendCapabilities::availableVideoCaptureDevices
     */
    class PHONONEXPERIMENTAL_EXPORT AvCapture : public QObject, public Phonon::MediaNode
    {
        Q_OBJECT
        P_DECLARE_PRIVATE(AvCapture)
        PHONON_OBJECT(AvCapture)
        Q_PROPERTY(Phonon::AudioCaptureDevice audioCaptureDevice READ audioCaptureDevice WRITE setAudioCaptureDevice)
        Q_PROPERTY(Phonon::VideoCaptureDevice videoCaptureDevice READ videoCaptureDevice WRITE setVideoCaptureDevice)
        public:
            /**
             * Constructs an AvCapture with the devices preferred for the specified
             * capture category.
             *
             * @param category Used to determine what devices are most suited for
             * the AvCapture.
             *
             * @see CaptureCategory
             */
            AvCapture(Phonon::CaptureCategory category, QObject *parent = NULL);

            /**
             * Returns the current state of the capture.
             *
             * @li If only the audio capture device is valid, it returns the audio capture state.
             * @li If only the video capture device is valid, it returns the video capture state.
             * @li If both the audio and video capture devices are valid, it only returns the
             * video capture state.
             */
            State state() const;

            /**
             * Returns the currently used capture source for the audio signal.
             */
            Phonon::AudioCaptureDevice audioCaptureDevice() const;

            /**
             * Returns the currently used capture source for the video signal.
             */
            Phonon::VideoCaptureDevice videoCaptureDevice() const;

            /**
             * Sets both the video and audio devices to the ones most suited for the
             * specified category
             *
             * @param category Used to determine what devices are most suited for
             * the AvCapture.
             */
            void setCaptureDevices(Phonon::CaptureCategory category);

            /**
             * Sets the audio capture source to use.
             *
             * @param source An object of class AudioCaptureDevice.
             *
             */
            void setAudioCaptureDevice(const Phonon::AudioCaptureDevice &source);

            /**
             * Sets the audio capture device by using a capture category to get
             * the appropriate device.
             *
             * @param category Capture category to use for getting a device
             */
            void setAudioCaptureDevice(Phonon::CaptureCategory category);

            PHONON_DEPRECATED void setAudioCaptureDevice(Phonon::Category category);

            /**
             * Sets the video capture source to use.
             *
             * @param source An object of class VideoCaptureDevice.
             *
             * @see videoCaptureDevice
             * @see setVideoCaptureDevice(int)
             */
            void setVideoCaptureDevice(const Phonon::VideoCaptureDevice &source);

            /**
             * Sets the audio capture device by using a capture category to get
             * the appropriate device.
             *
             * @param category Capture category to use for getting a device
             */
            void setVideoCaptureDevice(Phonon::CaptureCategory category);

            PHONON_DEPRECATED void setVideoCaptureDevice(Phonon::Category category);

            /**
             * @deprecated since 4.4.3, use
             * setVideoCaptureDevice(const Phonon::VideoCaptureDevice &source) instead
             */
            PHONON_DEPRECATED void setVideoCaptureDevice(const Phonon::Experimental::VideoCaptureDevice &source);

        public Q_SLOTS:
            /**
             * Start capture.
             */
            void start();

            /**
             * Pause capture.
             */
            void pause();

            /**
             * Stop capture.
             */
            void stop();

        Q_SIGNALS:
            /**
             * Emitted when the state of the video or audio capture device has been changed.
             *
             * \li If only the audio capture device is valid, it notifies about the audio capture state.
             * \li If only the video capture device is valid, it notifies about the video capture state.
             * \li If both the audio and video capture devices are valid, it only notifies about the
             * video capture state.
             */
            void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    };

} // namespace Experimental
} // namespace Phonon

#endif // NO_PHONON_AVCAPTURE

#endif // PHONON_EXPERIMENTAL_AVCAPTURE_H
