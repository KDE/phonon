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

#include "export.h"
#include "../medianode.h"
#include "../phonondefs.h"
#include "objectdescription.h"
#include "../phononnamespace.h"

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
     * well as for video. The list of available sources is available through
     * BackendCapabilities::availableAudioCaptureDevices and
     * BackendCapabilities::availableVideoCaptureDevices.
     *
     * @ingroup Recording
     * @author Matthias Kretz <kretz@kde.org>
     * @see BackendCapabilities::availableAudioCaptureDevices
     * @see BackendCapabilities::availableVideoCaptureDevices
     */
    class PHONONEXPERIMENTAL_EXPORT AvCapture : public QObject, public Phonon::MediaNode
    {
        Q_OBJECT
        K_DECLARE_PRIVATE(AvCapture)
        PHONON_OBJECT(AvCapture)
        Q_PROPERTY(AudioCaptureDevice audioCaptureDevice READ audioCaptureDevice WRITE setAudioCaptureDevice)
        Q_PROPERTY(VideoCaptureDevice videoCaptureDevice READ videoCaptureDevice WRITE setVideoCaptureDevice)
        public:
            /**
             * Returns the currently used capture source for the audio signal.
             *
             * @see AudioCaptureDevice
             * @see setAudioCaptureDevice(const AudioCaptureDevice &)
             * @see setAudioCaptureDevice(int)
             */
            AudioCaptureDevice audioCaptureDevice() const;

            /**
             * Returns the currently used capture source for the video signal.
             *
             * @see VideoCaptureDevice
             * @see setVideoCaptureDevice(const VideoCaptureDevice &)
             * @see setVideoCaptureDevice(int)
             */
            VideoCaptureDevice videoCaptureDevice() const;

            /**
             * Sets the audio capture source to use.
             *
             * @param source An object of class AudioCaptureDevice. A list of
             * available objects can be queried from
             * BackendCapabilities::availableAudioCaptureDevices.
             *
             * @see audioCaptureDevice
             * @see setAudioCaptureDevice(int)
             */
            void setAudioCaptureDevice(const AudioCaptureDevice &source);
            void setAudioCaptureDevice(Phonon::Category category);

            /**
             * Sets the video capture source to use.
             *
             * @param source An object of class VideoCaptureDevice. A list of
             * available objects can be queried from
             * BackendCapabilities::availableVideoCaptureDevices.
             *
             * @see videoCaptureDevice
             * @see setVideoCaptureDevice(int)
             */
            void setVideoCaptureDevice(const VideoCaptureDevice &source);
            void setVideoCaptureDevice(Phonon::Category category);

        public Q_SLOTS:
            /**
             * Start capture.
             */
            void start();

            /**
             * Stop capture.
             */
            void stop();
    };

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_EXPERIMENTAL_AVCAPTURE_H
