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

#include "objectdescription.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>

class QUrl;
class QIODevice;

namespace Phonon
{

class SourcePrivate;
class AbstractMediaStream;

class PHONON_EXPORT Source
{
    friend class StreamInterface;
    friend QDebug operator <<(QDebug dbg, const Phonon::Source &);
public:
    enum Type {
        Invalid = -1,
        LocalFile,
        Url,
        Disc,
        Stream,
        CaptureDevice,
        Empty,
        AudioVideoCapture
    };

    Source();
    Source(const QUrl &url);
    Source(DiscType discType, const QString &deviceName = QString());
    Source(const AudioCaptureDevice& device);
    Source(const VideoCaptureDevice& device);
    Source(Capture::DeviceType deviceType, CaptureCategory category = NoCaptureCategory);
    Source(CaptureCategory category);

    Source(AbstractMediaStream *stream);
    Source(QIODevice *ioDevice);
    ~Source();
    Source(const Source &rhs);
    Source &operator=(const Source &rhs);
    bool operator==(const Source &rhs) const;
    void setAutoDelete(bool enable);
    bool autoDelete() const;
    Type type() const;
    QString fileName() const;
    QUrl url() const;
    DiscType discType() const;
    const DeviceAccessList& deviceAccessList() const;
    const DeviceAccessList& videoDeviceAccessList() const;
    const DeviceAccessList& audioDeviceAccessList() const;
    QString deviceName() const;
    AbstractMediaStream *stream() const;
    AudioCaptureDevice audioCaptureDevice() const;
    VideoCaptureDevice videoCaptureDevice() const;

protected:
    QExplicitlySharedDataPointer<SourcePrivate> d;
    Source(SourcePrivate &);
};

PHONON_EXPORT QDebug operator <<(QDebug dbg, const Phonon::Source &);

} // namespace Phonon

#endif // PHONON_MEDIASOURCE_H
