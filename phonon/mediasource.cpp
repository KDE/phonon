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

#include "mediasource.h"
#include "mediasource_p.h"
#include "iodevicestream_p.h"
#include "abstractmediastream_p.h"
#include "globalconfig.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>

namespace Phonon
{

MediaSource::MediaSource(MediaSourcePrivate &dd)
    : d(&dd)
{
}

MediaSource::MediaSource()
    : d(new MediaSourcePrivate(Empty))
{
}

MediaSource::MediaSource(const QString &filename)
    : d(new MediaSourcePrivate(LocalFile))
{
    if (filename.startsWith(QLatin1String(":/")) || filename.startsWith(QLatin1String("qrc:///"))) {
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        d->url.setScheme("qrc");
        d->url.setPath(filename.mid(filename.startsWith(QLatin1Char(':')) ? 1 : 6));

        // QFile needs :/ syntax
        QString path(QLatin1Char(':') + d->url.path());

        if (QFile::exists(path)) {
            d->type = Stream;
            d->ioDevice = new QFile(path);
            d->setStream(new IODeviceStream(d->ioDevice, d->ioDevice));
        } else {
            d->type = Invalid;
        }
#else
        d->type = Invalid;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
    } else {
        const QFileInfo fileinfo(filename);
        if (fileinfo.exists()) {
            d->url = QUrl::fromLocalFile(fileinfo.absoluteFilePath());
            if (!d->url.host().isEmpty()) {
                // filename points to a file on a network share (eg \\host\share\path)
                d->type = Url;
            }
        } else {
            d->url = filename;
            if (d->url.isValid()) {
                d->type = Url;
            } else {
                d->type = Invalid;
            }
        }
    }
}

MediaSource::MediaSource(const QUrl &url)
    : d(new MediaSourcePrivate(Url))
{
    if (url.isValid()) {
        if (url.scheme() == QLatin1String("qrc")) {
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            // QFile needs :/ syntax
            QString path(QLatin1Char(':') + url.path());

            if (QFile::exists(path)) {
                d->type = Stream;
                d->ioDevice = new QFile(path);
                d->setStream(new IODeviceStream(d->ioDevice, d->ioDevice));
            } else {
                d->type = Invalid;
            }
#else
            d->type = Invalid;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
        }
        d->url = url;
    } else {
        d->type = Invalid;
    }
}

MediaSource::MediaSource(DiscType dt, const QString &deviceName)
    : d(new MediaSourcePrivate(Disc))
{
    if (dt == NoDisc) {
        d->type = Invalid;
        return;
    }
    d->discType = dt;
    d->deviceName = deviceName;
}

// NOTE: this is deprecated
MediaSource::MediaSource(const DeviceAccess &)
    : d(new MediaSourcePrivate(Invalid))
{
}

#ifndef PHONON_NO_AUDIOCAPTURE
MediaSource::MediaSource(const AudioCaptureDevice& device)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    d->setCaptureDevices(device, VideoCaptureDevice());
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
MediaSource::MediaSource(const VideoCaptureDevice& device)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    d->setCaptureDevices(AudioCaptureDevice(), device);
}
#endif //PHONON_NO_VIDEOCAPTURE

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
MediaSource::MediaSource(CaptureCategory category)
    : d(new MediaSourcePrivate(AudioVideoCapture))
{
    d->setCaptureDevices(category);
}

MediaSource::MediaSource(Capture::DeviceType deviceType, CaptureCategory category)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    d->setCaptureDevice(deviceType, category);
}
#endif // !PHONON_NO_VIDEOCAPTURE && !PHONON_NO_AUDIOCAPTURE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
MediaSource::MediaSource(AbstractMediaStream *stream)
    : d(new MediaSourcePrivate(Stream))
{
    if (stream) {
        d->setStream(stream);
    } else {
        d->type = Invalid;
    }
}

MediaSource::MediaSource(QIODevice *ioDevice)
    : d(new MediaSourcePrivate(Stream))
{
    if (ioDevice) {
        d->setStream(new IODeviceStream(ioDevice, ioDevice));
        d->ioDevice = ioDevice;
    } else {
        d->type = Invalid;
    }
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

/* post 4.0
MediaSource::MediaSource(const QList<MediaSource> &mediaList)
    : d(new MediaSourcePrivate(Link))
{
    d->linkedSources = mediaList;
    foreach (MediaSource ms, mediaList) {
        Q_ASSERT(ms.type() != Link);
    }
}

QList<MediaSource> MediaSource::substreams() const
{
    return d->linkedSources;
}
*/

MediaSource::~MediaSource()
{
}

MediaSourcePrivate::~MediaSourcePrivate()
{
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (autoDelete) {
        //here we use deleteLater because this object
        //might be destroyed from another thread
        if (stream)
            stream->deleteLater();
        if (ioDevice)
            ioDevice->deleteLater();
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
}

MediaSource::MediaSource(const MediaSource &rhs)
    : d(rhs.d)
{
}

MediaSource &MediaSource::operator=(const MediaSource &rhs)
{
    d = rhs.d;
    return *this;
}

bool MediaSource::operator==(const MediaSource &rhs) const
{
    return d == rhs.d;
}

void MediaSource::setAutoDelete(bool autoDelete)
{
    d->autoDelete = autoDelete;
}

bool MediaSource::autoDelete() const
{
    return d->autoDelete;
}

MediaSource::Type MediaSource::type() const
{
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (d->type == Stream && d->stream == 0) {
        return Invalid;
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
    return d->type;
}

QString MediaSource::fileName() const
{
    return d->url.toLocalFile();
}

Mrl MediaSource::mrl() const
{
    return Mrl(d->url);
}

QUrl MediaSource::url() const
{
    return d->url;
}

DiscType MediaSource::discType() const
{
    return d->discType;
}

const DeviceAccessList& MediaSource::deviceAccessList() const
{
#ifndef PHONON_NO_AUDIOCAPTURE
    if (d->audioCaptureDevice.isValid())
        return d->audioDeviceAccessList;
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
    if (d->videoCaptureDevice.isValid())
        return d->videoDeviceAccessList;
#endif

    return d->audioDeviceAccessList;    // It should be invalid
}

const DeviceAccessList& MediaSource::audioDeviceAccessList() const
{
    return d->audioDeviceAccessList;
}

const DeviceAccessList& MediaSource::videoDeviceAccessList() const
{
    return d->videoDeviceAccessList;
}

QString MediaSource::deviceName() const
{
    return d->deviceName;
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
AbstractMediaStream *MediaSource::stream() const
{
    return d->stream;
}

void MediaSourcePrivate::setStream(AbstractMediaStream *s)
{
    stream = s;
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

#ifndef PHONON_NO_AUDIOCAPTURE
AudioCaptureDevice MediaSource::audioCaptureDevice() const
{
    return d->audioCaptureDevice;
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
VideoCaptureDevice MediaSource::videoCaptureDevice() const
{
    return d->videoCaptureDevice;
}
#endif //PHONON_NO_VIDEOCAPTURE

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
void MediaSourcePrivate::setCaptureDevice(Capture::DeviceType deviceType, CaptureCategory category)
{
    switch (deviceType) {
        case Capture::VideoType: {
            setCaptureDevices(AudioCaptureDevice(),
                VideoCaptureDevice::fromIndex(GlobalConfig().videoCaptureDeviceFor(category)));
            break;
        }
        case Capture::AudioType: {
            setCaptureDevices(
                AudioCaptureDevice::fromIndex(GlobalConfig().audioCaptureDeviceFor(category)), VideoCaptureDevice());
            break;
        }
    }
}

void MediaSourcePrivate::setCaptureDevices(CaptureCategory category)
{
    setCaptureDevices(
        AudioCaptureDevice::fromIndex(GlobalConfig().audioCaptureDeviceFor(category)),
        VideoCaptureDevice::fromIndex(GlobalConfig().videoCaptureDeviceFor(category)));
}

void MediaSourcePrivate::setCaptureDevices(const AudioCaptureDevice &audioDevice, const VideoCaptureDevice &videoDevice)
{
    audioCaptureDevice = audioDevice;
    videoCaptureDevice = videoDevice;

    if (audioDevice.propertyNames().contains("deviceAccessList") &&
            !audioDevice.property("deviceAccessList").value<DeviceAccessList>().isEmpty()) {
        audioDeviceAccessList = audioDevice.property("deviceAccessList").value<DeviceAccessList>();
    }

    if (videoDevice.propertyNames().contains("deviceAccessList") &&
            !videoDevice.property("deviceAccessList").value<DeviceAccessList>().isEmpty()) {
        videoDeviceAccessList = videoDevice.property("deviceAccessList").value<DeviceAccessList>();
    }

    bool validAudio = !audioDeviceAccessList.isEmpty();
    bool validVideo = !videoDeviceAccessList.isEmpty();
    type = MediaSource::Invalid;
    if (validAudio && validVideo)
        type = MediaSource::AudioVideoCapture;
    else if (validAudio || validVideo)
        type = MediaSource::CaptureDevice;
}
#endif // !PHONON_NO_VIDEOCAPTURE && !PHONON_NO_AUDIOCAPTURE

QDebug operator <<(QDebug dbg, const Phonon::MediaSource &source)
{
    switch (source.type()) {
    case MediaSource::Invalid:
        dbg.nospace() << "Invalid()";
        break;
    case MediaSource::LocalFile:
        dbg.nospace() << "LocalFile(" << source.url() << ")";
        break;
    case MediaSource::Url:
        dbg.nospace() << "Url(" << source.url() << ")";
        break;
    case MediaSource::Disc:
        dbg.nospace() << "Disc(";
        switch (source.discType()) {
        case NoDisc:
            dbg.nospace() << "NoDisc";
            break;
        case Cd:
            dbg.nospace() << "Cd: " << source.deviceName();
            break;
        case Dvd:
            dbg.nospace() << "Dvd: " << source.deviceName();
            break;
        case Vcd:
            dbg.nospace() << "Vcd: " << source.deviceName();
            break;
        case BluRay:
            dbg.nospace() << "BluRay: " << source.deviceName();
            break;
        }
        dbg.nospace() << ")";
        break;
    case MediaSource::Stream: {
        dbg.nospace() << "Stream(IOAddr: " << source.d->ioDevice;
        QObject *qiodevice = qobject_cast<QObject *>(source.d->ioDevice);
        if (qiodevice)
            dbg.nospace() << " IOClass: " << qiodevice->metaObject()->className();

        dbg.nospace() << "; StreamAddr: " << source.stream();
        QObject *qstream = qobject_cast<QObject *>(source.stream());
        if (qstream)
            dbg.nospace() << " StreamClass: " << qstream->metaObject()->className();

        dbg.nospace() << ")";
        break;
    }
    case MediaSource::CaptureDevice:
    case MediaSource::AudioVideoCapture:
        dbg.nospace() << "AudioVideoCapture(A:" << source.audioCaptureDevice().name()
                      << "/V: " << source.videoCaptureDevice().name() << ")";
        break;
    case MediaSource::Empty:
        dbg.nospace() << "Empty()";
        break;
    }

    return dbg.maybeSpace();
}

} // namespace Phonon

// vim: sw=4 sts=4 et tw=100
