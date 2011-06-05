/*  This file is part of the KDE project
    Copyright (C) 2011 Casian Andrei <skeletk13@gmail.com>

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

#ifndef PHONON_AVCAPTURE_H
#define PHONON_AVCAPTURE_H

#ifndef PHONON_NO_CAPTURE

#include "phonon_export.h"
#include "phonondefs.h"
#include "phononnamespace.h"
#include "medianode.h"
#include "objectdescription.h"

namespace Phonon
{
    class AvCapturePrivate;

    /**
     * @short Media node with both video and audio capture capabilities
     *
     * This class gives you access to the capture capabilities of the backend,
     * specifically to capture both video and audio at the same time, from
     * different devices.
     *
     * It is essentialy a tuple of a MediaObject for video capture and a
     * MediaObject for audio capture.
     *
     * There might be more than only one possible capture source, for audio, as
     * well as for video.
     *
     * @note Some devices may be able to capture audio/video using their assigned
     * media object. In that case, you don't need to use AvCapture.
     *
     * @ingroup Recording
     * @see BackendCapabilities::availableAudioCaptureDevices
     * @see BackendCapabilities::availableVideoCaptureDevices
     */
    class PHONON_EXPORT AvCapture : public QObject, public MediaNode
    {
        Q_OBJECT
        K_DECLARE_PRIVATE(AvCapture)
        PHONON_OBJECT(AvCapture)
        Q_PROPERTY(AudioCaptureDevice audioCaptureDevice READ audioCaptureDevice WRITE setAudioCaptureDevice)
        Q_PROPERTY(VideoCaptureDevice videoCaptureDevice READ videoCaptureDevice WRITE setVideoCaptureDevice)

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
            AvCapture(CaptureCategory category, QObject *parent = NULL);

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
            AudioCaptureDevice audioCaptureDevice() const;

            /**
             * Returns the currently used capture source for the video signal.
             */
            VideoCaptureDevice videoCaptureDevice() const;

            /**
             * Sets both the video and audio devices to the ones most suited for the
             * specified category
             *
             * @param category Used to determine what devices are most suited for
             * the AvCapture.
             */
            void setCaptureDevices(CaptureCategory category);

            /**
             * Sets the audio capture source to use.
             *
             * @param source An object of class AudioCaptureDevice.
             *
             */
            void setAudioCaptureDevice(const AudioCaptureDevice &source);

            /**
             * Sets the audio capture device by using a capture category to get
             * the appropriate device.
             *
             * @param category Capture category to use for getting a device
             */
            void setAudioCaptureDevice(CaptureCategory category);

            /**
             * Sets the video capture source to use.
             *
             * @param source An object of class VideoCaptureDevice.
             *
             * @see videoCaptureDevice
             * @see setVideoCaptureDevice(int)
             */
            void setVideoCaptureDevice(const VideoCaptureDevice &source);

            /**
             * Sets the audio capture device by using a capture category to get
             * the appropriate device.
             *
             * @param category Capture category to use for getting a device
             */
            void setVideoCaptureDevice(CaptureCategory category);

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
             * @li If only the audio capture device is valid, it notifies about the audio capture state.
             * @li If only the video capture device is valid, it notifies about the video capture state.
             * @li If both the audio and video capture devices are valid, it only notifies about the
             * video capture state.
             */
            void stateChanged(State newstate, State oldstate);
    };

} // namespace Phonon

#endif // PHONON_NO_CAPTURE

#endif // PHONON_AVCAPTURE_H
