/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_MEDIASOURCE_H
#define PHONON_MEDIASOURCE_H

#include "phonon_export.h"
#include "phononnamespace.h"

#include "mrl.h"
#include "objectdescription.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>


class QUrl;
class QIODevice;

namespace Phonon
{

class MediaSourcePrivate;
class AbstractMediaStream;

/** \class MediaSource mediasource.h phonon/MediaSource
 * Note that all constructors of this class are implicit, so that you can simply write
 * \code
 * MediaObject m;
 * QString fileName("/home/foo/bar.ogg");
 * QUrl url("http://www.example.com/stream.mp3");
 * QBuffer *someBuffer;
 * m.setCurrentSource(fileName);
 * m.setCurrentSource(url);
 * m.setCurrentSource(someBuffer);
 * m.setCurrentSource(Phonon::Cd);
 * \endcode
 *
 * \ingroup Playback
 * \ingroup Recording
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT MediaSource
{
    friend class StreamInterface;
    friend PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::MediaSource &);
    public:
        /**
         * Identifies the type of media described by the MediaSource object.
         *
         * \see MediaSource::type()
         */
        enum Type {
            /**
             * The MediaSource object does not describe any valid source.
             */
            Invalid = -1,
            /**
             * The MediaSource object describes a local file.
             */
            LocalFile,
            /**
             * The MediaSource object describes a URL, which can be both a local file and a file on
             * the network.
             */
            Url,
            /**
             * The MediaSource object describes a disc.
             */
            Disc,
            /**
             * The MediaSource object describes a data stream.
             *
             * This is also the type used for QIODevices.
             *
             * \see AbstractMediaStream
             */
            Stream,
            /**
            * The MediaSource object describes a single capture device.
            * This could be either audio or video.
            */
            CaptureDevice,
            /**
             * An empty MediaSource.
             *
             * It can be used to unload the current media from a MediaObject.
             *
             * \see MediaSource()
             */
            Empty,
            /**
             * The MediaSource object describes one device for video capture and one for audio
             * capture. Facilitates capturing both audio and video at the same time, from
             * different devices.
             * It's essentially like two CaptureDevice media sources (one of video type, one
             * of audio type) merged together.
             */
            AudioVideoCapture
/*          post 4.0:
            / **
             * Links multiple MediaSource objects together.
             * /
            Link
*/
        };

        /**
         * Creates an empty MediaSource.
         *
         * An empty MediaSource is considered valid and can be set on a MediaObject to unload its
         * current media.
         *
         * \see Empty
         */
        MediaSource();

        /**
         * Creates a MediaSource object for a local file or a Qt resource.
         *
         * \deprecated Use MediaSource(QUrl("qrc:///...")) for a Qt resource, MediaSource(QUrl::fromLocalFile("...")) for a local file, or MediaSource(QUrl("...")) for an URL.
         *
         * \param fileName file name of a local media file or a Qt resource that was compiled in.
         */
        PHONON_DEPRECATED MediaSource(const QString &fileName); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for a URL.
         *
         * A Qt resource can be specified by using an url with a qrc scheme.
         *
         * \param url URL to a media file or stream.
         */
        MediaSource(const QUrl &url); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for discs.
         *
         * \param discType See \ref DiscType
         * \param deviceName A platform dependent device name. This can be useful if the computer
         * has more than one CD drive. It is recommended to use Solid to retrieve the device name in
         * a portable way.
         */
        MediaSource(DiscType discType, const QString &deviceName = QString()); //krazy:exclude=explicit

#ifndef PHONON_NO_AUDIOCAPTURE
        /**
        * Creates a MediaSource object for audio capture devices.
        * If the device is valid, this creates a 'CaptureDevice' type MediaSource.
        */
        MediaSource(const AudioCaptureDevice& device);
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
        /**
        * Creates a MediaSource object for video capture devices.
        * If the device is valid, this creates a 'CaptureDevice' type MediaSource
        */
        MediaSource(const VideoCaptureDevice& device);
#endif

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
        /**
         * Sets the source to the preferred audio capture device for the specified category
         * If a valid device is found, this creates a 'CaptureDevice' type MediaSource
         */
        MediaSource(Capture::DeviceType deviceType, CaptureCategory category = NoCaptureCategory);

        /**
         * Creates a MediaSource object that tries to describe a video capture device and
         * an audio capture device, together. The devices are appropriate for the specified
         * category.
         *
         * If valid devices are found for both audio and video, then the resulting MediaSource
         * is of type 'AudioVideoCapture'. If only an audio or a video valid device is found,
         * the resulting type is 'CaptureDevice'. If no valid devices are found, the resulting
         * type is 'Invalid'.
         */
        MediaSource(CaptureCategory category);
#endif

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        /**
         * Creates a MediaSource object for a data stream.
         *
         * Your application can provide the media data by subclassing AbstractMediaStream and
         * passing a pointer to that object. %Phonon will never delete the \p stream.
         *
         * \param stream The AbstractMediaStream subclass to provide the media data.
         *
         * \see setAutoDelete
         */
        MediaSource(AbstractMediaStream *stream); //krazy:exclude=explicit

        /**
         * Creates a MediaSource object for a QIODevice.
         *
         * This constructor can be very handy in the combination of QByteArray and QBuffer.
         *
         * \param ioDevice An arbitrary readable QIODevice subclass. If the device is not opened
         * MediaSource will open it as QIODevice::ReadOnly. Sequential I/O devices are possible,
         * too. For those MediaObject::isSeekable() will have to return false obviously.
         *
         * \see setAutoDelete
         */
        MediaSource(QIODevice *ioDevice); //krazy:exclude=explicit
#endif

        /**
         * Destroys the MediaSource object.
         */
        ~MediaSource();

        /**
         * Constructs a copy of \p rhs.
         *
         * This constructor is fast thanks to explicit sharing.
         */
        MediaSource(const MediaSource &rhs);

        /**
         * Assigns \p rhs to this MediaSource and returns a reference to this MediaSource.
         *
         * This operation is fast thanks to explicit sharing.
         */
        MediaSource &operator=(const MediaSource &rhs);

        /**
         * Returns \p true if this MediaSource is equal to \p rhs; otherwise returns \p false.
         */
        bool operator==(const MediaSource &rhs) const;

        /**
         * Tell the MediaSource to take ownership of the AbstractMediaStream or QIODevice that was
         * passed in the constructor.
         *
         * The default setting is \p false, for safety. If you turn it on, you should only access
         * the AbstractMediaStream/QIODevice object as long as you yourself keep a MediaSource
         * object around. As long as you keep the MediaSource object wrapping the stream/device
         * the object will not get deleted.
         *
         * \see autoDelete
         */
        void setAutoDelete(bool enable);

        /**
         * Returns the setting of the auto-delete option. The default is \p false.
         *
         * \see setAutoDelete
         */
        bool autoDelete() const;

        /**
         * Returns the type of the MediaSource (depends on the constructor that was used).
         *
         * \see Type
         */
        Type type() const;

        /**
         * Returns the file name of the MediaSource if type() == LocalFile; otherwise returns
         * QString().
         */
        QString fileName() const;

        /**
         * Returns the MRL of the MediaSource if type() == URL or type() == LocalFile; otherwise
         * returns Mrl().
         * Phonon::Mrl is based on QUrl and adds some additional functionality that
         * is necessary to ensure proper encoding usage in the Phonon backends.
         *
         * Usually you will not have to use this in an application.
         *
         * \since 4.5
         * \ingroup Backend
         */
        Mrl mrl() const;

        /**
         * Returns the url of the MediaSource if type() == URL or type() == LocalFile; otherwise
         * returns QUrl().
         */
        QUrl url() const;

        /**
         * Returns the disc type of the MediaSource if type() == Disc; otherwise returns \ref
         * NoDisc.
         */
        DiscType discType() const;

        /**
         * Returns the access list for the device of this media source. Valid for capture devices.
         * \warning use only with MediaSource with type() == CaptureDevice
         */
        const DeviceAccessList& deviceAccessList() const;

        /**
         * Returns the access list for the video device used for capture.
         * Valid for type() == CaptureDevice or type() == AudioVideoCapture.
         * If used with CaptureDevice, the kind of device should be Video, for a valid result.
         */
        const DeviceAccessList& videoDeviceAccessList() const;

        /**
         * Returns the access list for the audio device used for capture.
         * Valid for type() == CaptureDevice or type() == AudioVideoCapture.
         * If used with CaptureDevice, the kind of device should be Audio, for a valid result.
         */
        const DeviceAccessList& audioDeviceAccessList() const;

        /**
         * Returns the device name of the MediaSource if type() == Disc; otherwise returns
         * QString().
         */
        QString deviceName() const;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        /**
         * Returns the media stream of the MediaSource if type() == Stream; otherwise returns 0.
         * QIODevices are handled as streams, too.
         */
        AbstractMediaStream *stream() const;
#endif

#ifndef PHONON_NO_AUDIOCAPTURE
        /**
         * Returns the audio capture device for the media source if applicable.
         */
        AudioCaptureDevice audioCaptureDevice() const;
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
        /**
         * Returns the video capture device for the media source if applicable.
         */
        VideoCaptureDevice videoCaptureDevice() const;
#endif

/*      post 4.0:
        MediaSource(const QList<MediaSource> &mediaList);
        QList<MediaSource> substreams() const;
*/

    protected:
        QExplicitlySharedDataPointer<MediaSourcePrivate> d;
        MediaSource(MediaSourcePrivate &);

        PHONON_DEPRECATED MediaSource(const DeviceAccess &access);
};

PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::MediaSource &);

} // namespace Phonon


#endif // PHONON_MEDIASOURCE_H
