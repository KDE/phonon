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

#include "source.h"
#include "source_p.h"
#include "iodevicestream_p.h"
#include "abstractmediastream_p.h"
#include "globalconfig.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFile>

namespace Phonon
{

Source::Source(SourcePrivate &dd)
    : d(&dd)
{
}

Source::Source()
    : d(new SourcePrivate(Empty))
{
}

Source::Source(const QUrl &url)
    : d(new SourcePrivate(Url))
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

Source::Source(DiscType dt, const QString &deviceName)
    : d(new SourcePrivate(Disc))
{
    if (dt == NoDisc) {
        d->type = Invalid;
        return;
    }
    d->discType = dt;
    d->deviceName = deviceName;
}

#ifndef PHONON_NO_AUDIOCAPTURE
Source::Source(const AudioCaptureDevice& device)
    : d(new SourcePrivate(CaptureDevice))
{
    d->setCaptureDevices(device, VideoCaptureDevice());
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
Source::Source(const VideoCaptureDevice& device)
    : d(new SourcePrivate(CaptureDevice))
{
    d->setCaptureDevices(AudioCaptureDevice(), device);
}
#endif //PHONON_NO_VIDEOCAPTURE

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
Source::Source(CaptureCategory category)
    : d(new SourcePrivate(AudioVideoCapture))
{
    d->setCaptureDevices(category);
}

Source::Source(Capture::DeviceType deviceType, CaptureCategory category)
    : d(new SourcePrivate(CaptureDevice))
{
    d->setCaptureDevice(deviceType, category);
}
#endif // !PHONON_NO_VIDEOCAPTURE && !PHONON_NO_AUDIOCAPTURE

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
Source::Source(AbstractMediaStream *stream)
    : d(new SourcePrivate(Stream))
{
    if (stream) {
        d->setStream(stream);
    } else {
        d->type = Invalid;
    }
}

Source::Source(QIODevice *ioDevice)
    : d(new SourcePrivate(Stream))
{
    if (ioDevice) {
        d->setStream(new IODeviceStream(ioDevice, ioDevice));
        d->ioDevice = ioDevice;
    } else {
        d->type = Invalid;
    }
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

Source::~Source()
{
}

SourcePrivate::~SourcePrivate()
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

Source::Source(const Source &rhs)
    : d(rhs.d)
{
}

Source &Source::operator=(const Source &rhs)
{
    d = rhs.d;
    return *this;
}

bool Source::operator==(const Source &rhs) const
{
    return d == rhs.d;
}

void Source::setAutoDelete(bool autoDelete)
{
    d->autoDelete = autoDelete;
}

bool Source::autoDelete() const
{
    return d->autoDelete;
}

Source::Type Source::type() const
{
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    if (d->type == Stream && d->stream == 0) {
        return Invalid;
    }
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
    return d->type;
}

QString Source::fileName() const
{
    return d->url.toLocalFile();
}

QUrl Source::url() const
{
    return d->url;
}

DiscType Source::discType() const
{
    return d->discType;
}

const DeviceAccessList& Source::deviceAccessList() const
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

const DeviceAccessList& Source::audioDeviceAccessList() const
{
    return d->audioDeviceAccessList;
}

const DeviceAccessList& Source::videoDeviceAccessList() const
{
    return d->videoDeviceAccessList;
}

QString Source::deviceName() const
{
    return d->deviceName;
}

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
AbstractMediaStream *Source::stream() const
{
    return d->stream;
}

void SourcePrivate::setStream(AbstractMediaStream *s)
{
    stream = s;
}
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

#ifndef PHONON_NO_AUDIOCAPTURE
AudioCaptureDevice Source::audioCaptureDevice() const
{
    return d->audioCaptureDevice;
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
VideoCaptureDevice Source::videoCaptureDevice() const
{
    return d->videoCaptureDevice;
}
#endif //PHONON_NO_VIDEOCAPTURE

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
void SourcePrivate::setCaptureDevice(Capture::DeviceType deviceType, CaptureCategory category)
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

void SourcePrivate::setCaptureDevices(CaptureCategory category)
{
    setCaptureDevices(
        AudioCaptureDevice::fromIndex(GlobalConfig().audioCaptureDeviceFor(category)),
        VideoCaptureDevice::fromIndex(GlobalConfig().videoCaptureDeviceFor(category)));
}

void SourcePrivate::setCaptureDevices(const AudioCaptureDevice &audioDevice, const VideoCaptureDevice &videoDevice)
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
    type = Source::Invalid;
    if (validAudio && validVideo)
        type = Source::AudioVideoCapture;
    else if (validAudio || validVideo)
        type = Source::CaptureDevice;
}
#endif // !PHONON_NO_VIDEOCAPTURE && !PHONON_NO_AUDIOCAPTURE

QDebug operator <<(QDebug dbg, const Phonon::Source &source)
{
    switch (source.type()) {
    case Source::Invalid:
        dbg.nospace() << "Invalid()";
        break;
    case Source::LocalFile:
        dbg.nospace() << "LocalFile(" << source.url() << ")";
        break;
    case Source::Url:
        dbg.nospace() << "Url(" << source.url() << ")";
        break;
    case Source::Disc:
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
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
    case Source::Stream: {
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
#endif // QT_NO_PHONON_ABSTRACTMEDIASTREAM
    case Source::CaptureDevice:
    case Source::AudioVideoCapture:
        dbg.nospace() << "AudioVideoCapture(A:" << source.audioCaptureDevice().name()
                      << "/V: " << source.videoCaptureDevice().name() << ")";
        break;
    case Source::Empty:
        dbg.nospace() << "Empty()";
        break;
    }

    return dbg.maybeSpace();
}

} // namespace Phonon
