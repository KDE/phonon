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
#include <QtCore/QFSFileEngine>

QT_BEGIN_NAMESPACE

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
    const QFileInfo fileInfo(filename);
    if (fileInfo.exists()) {
        bool localFs = QAbstractFileEngine::LocalDiskFlag & QFSFileEngine(filename).fileFlags(QAbstractFileEngine::LocalDiskFlag);
        if (localFs && !filename.startsWith(QLatin1String(":/")) && !filename.startsWith(QLatin1String("qrc://"))) {
            d->url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
        } else {
            // it's a Qt resource -> use QFile
            d->type = Stream;
            d->ioDevice = new QFile(filename);
            d->setStream(new IODeviceStream(d->ioDevice, d->ioDevice));
            d->url =  QUrl::fromLocalFile(fileInfo.absoluteFilePath());
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

MediaSource::MediaSource(const QUrl &url)
    : d(new MediaSourcePrivate(Url))
{
    if (url.isValid()) {
        d->url = url;
    } else {
        d->type = Invalid;
    }
}

MediaSource::MediaSource(Phonon::DiscType dt, const QString &deviceName)
    : d(new MediaSourcePrivate(Disc))
{
    if (dt == NoDisc) {
        d->type = Invalid;
        return;
    }
    d->discType = dt;
    d->deviceName = deviceName;
}

// NOTE: this is a protected constructor
MediaSource::MediaSource(const DeviceAccess &access)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    if (access.first.isEmpty() || access.second.isEmpty()) {
        d->type = Invalid;
        return;
    }
    d->deviceAccessList.append(access);
}

#if !defined(PHONON_NO_AUDIOCAPTURE) && !defined(PHONON_NO_VIDEOCAPTURE)
#endif //PHONON_NO_AUDIOCAPTURE && PHONON_NO_VIDEOCAPTURE

#ifndef PHONON_NO_AUDIOCAPTURE
MediaSource::MediaSource(const Phonon::AudioCaptureDevice& acDevice)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    setAudioCaptureDevice(acDevice);
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
MediaSource::MediaSource(const Phonon::VideoCaptureDevice& vcDevice)
    : d(new MediaSourcePrivate(CaptureDevice))
{
    setVideoCaptureDevice(vcDevice);
}
#endif //PHONON_NO_VIDEOCAPTURE

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
    if (autoDelete) {
        //here we use deleteLater because this object
        //might be destroyed from another thread
        if (stream)
            stream->deleteLater();
        if (ioDevice)
            ioDevice->deleteLater();
    }
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
    if (d->type == Stream && d->stream == 0) {
        return Invalid;
    }
    return d->type;
}

QString MediaSource::fileName() const
{
    return d->url.toLocalFile();
}

QUrl MediaSource::url() const
{
    return d->url;
}

Phonon::DiscType MediaSource::discType() const
{
    return d->discType;
}

const DeviceAccessList& MediaSource::deviceAccessList() const
{
    return d->deviceAccessList;
}

QString MediaSource::deviceName() const
{
    return d->deviceName;
}

AbstractMediaStream *MediaSource::stream() const
{
    return d->stream;
}

void MediaSourcePrivate::setStream(AbstractMediaStream *s)
{
    stream = s;
}

#ifndef PHONON_NO_AUDIOCAPTURE
AudioCaptureDevice MediaSource::audioCaptureDevice() const
{
    return d->audioCaptureDevice;
}

void MediaSource::setAudioCaptureDevice(const Phonon::AudioCaptureDevice& acDevice)
{
    d->audioCaptureDevice = acDevice;

    // Grab the device access list from the properties
    if (acDevice.propertyNames().contains("deviceAccessList") &&
            !acDevice.property("deviceAccessList").value<DeviceAccessList>().isEmpty()) {
        d->type = MediaSource::CaptureDevice;
        d->deviceAccessList = acDevice.property("deviceAccessList").value<DeviceAccessList>();
    } else {
        // Invalidate the media source
        d->type = Invalid;
    }
}

void MediaSource::setAudioCaptureDevice(CaptureCategory category)
{
    setAudioCaptureDevice(AudioCaptureDevice::fromIndex(GlobalConfig().audioCaptureDeviceFor(category)));
}

void MediaSource::setAudioCaptureDevice(Category category)
{
    setAudioCaptureDevice(static_cast<CaptureCategory>(category));
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
VideoCaptureDevice MediaSource::videoCaptureDevice() const
{
    return d->videoCaptureDevice;
}

void MediaSource::setVideoCaptureDevice(const Phonon::VideoCaptureDevice& vcDevice)
{
    d->videoCaptureDevice = vcDevice;

    // Grab the device access list from the properties
    if (vcDevice.propertyNames().contains("deviceAccessList") &&
            !vcDevice.property("deviceAccessList").value<DeviceAccessList>().isEmpty()) {
        d->type = MediaSource::CaptureDevice;
        d->deviceAccessList = vcDevice.property("deviceAccessList").value<DeviceAccessList>();
    } else {
        // Invalidate the media source
        d->type = Invalid;
    }
}

void MediaSource::setVideoCaptureDevice(CaptureCategory category)
{
    setVideoCaptureDevice(VideoCaptureDevice::fromIndex(GlobalConfig().videoCaptureDeviceFor(category)));
}

void MediaSource::setVideoCaptureDevice(Category category)
{
    setVideoCaptureDevice(static_cast<CaptureCategory>(category));
}
#endif //PHONON_NO_VIDEOCAPTURE

} // namespace Phonon

QT_END_NAMESPACE

// vim: sw=4 sts=4 et tw=100
