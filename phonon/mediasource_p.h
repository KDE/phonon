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

#ifndef MEDIASOURCE_P_H
#define MEDIASOURCE_P_H

#include "mediasource.h"
#include "abstractmediastream.h"
#include "objectdescription.h"
#include "mrl.h"

#include <QUrl>
#include <QString>
#include <QSharedData>
#include <QPointer>

class QFile;

namespace Phonon
{

class PHONON_EXPORT MediaSourcePrivate : public QSharedData
{
    public:
        MediaSourcePrivate(MediaSource::Type t)
            : type(t), discType(NoDisc),
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            stream(nullptr),
            ioDevice(nullptr),
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
            autoDelete(false)
        {
        }

        virtual ~MediaSourcePrivate();

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        void setStream(AbstractMediaStream *s);
#endif

        MediaSource::Type type;
        Mrl url;
        Phonon::DiscType discType;
        QString deviceName;                         // Used for discs
        Phonon::DeviceAccessList audioDeviceAccessList;
        Phonon::DeviceAccessList videoDeviceAccessList;

#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
        // The AbstractMediaStream(2) may be deleted at any time by the application. If that happens
        // stream will be 0 automatically, but streamEventQueue will stay valid as we hold a
        // reference to it. This is necessary to avoid a races when setting the MediaSource while
        // another thread deletes the AbstractMediaStream2. StreamInterface(2) will then just get a
        // StreamEventQueue where nobody answers.
        QPointer<AbstractMediaStream> stream;
        QIODevice *ioDevice;
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
        void setCaptureDevice(Capture::DeviceType deviceType, CaptureCategory category);
        void setCaptureDevices(CaptureCategory category);
        void setCaptureDevices(const AudioCaptureDevice &audioDevice, const VideoCaptureDevice &videoDevice);
#endif // !PHONON_NO_VIDEOCAPTURE && !PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_AUDIOCAPTURE
        AudioCaptureDevice audioCaptureDevice;
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
        VideoCaptureDevice videoCaptureDevice;
#endif

        //QList<MediaSource> linkedSources;
        bool autoDelete;
};

} // namespace Phonon

#endif // MEDIASOURCE_P_H
// vim: sw=4 sts=4 et tw=100


