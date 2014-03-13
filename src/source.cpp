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

#include <QtCore/QFile>

namespace Phonon
{

Source::Source(SourcePrivate &dd)
    : d(&dd)
{
}

Source::Source()
    : d(new SourcePrivate())
{
}

Source::Source(const Source &other)
    : d(other.d)
{
}

Source::Source(const QUrl &url)
    : d(new SourcePrivate())
{
    if (url.isValid()) {
        if (url.scheme() == QLatin1String("qrc")) {
            // QFile needs :/ syntax
            QString path(QLatin1Char(':') + url.path());

            if (QFile::exists(path)) {
                d->m_ioDevice = new QFile(path);
                d->setStream(new IODeviceStream(d->m_ioDevice, d->m_ioDevice));
            }
        }
        d->m_url = url;
    }
}

Source::Source(DeviceType deviceType, const QByteArray &deviceName)
    : d(new SourcePrivate())
{
    if (deviceType == NoDevice) {
        return;
    }
    d->m_deviceType = deviceType;
    d->m_deviceName = deviceName;
}

Source::Source(AbstractMediaStream *stream)
    : d(new SourcePrivate())
{
    if (stream) {
        d->setStream(stream);
    }
}

Source::Source(QIODevice *ioDevice)
    : d(new SourcePrivate())
{
    if (ioDevice) {
        d->setStream(new IODeviceStream(ioDevice, ioDevice));
        d->m_ioDevice = ioDevice;
    }
}

Source::~Source()
{
}

SourcePrivate::~SourcePrivate()
{
    //here we use deleteLater because this object
    //might be destroyed from another thread
    if (m_stream)
        m_stream->deleteLater();
    if (m_ioDevice)
        m_ioDevice->deleteLater();
}

Source &Source::operator=(const Source &other)
{
    d = other.d;
    return *this;
}

bool Source::operator==(const Source &other) const
{
    return d == other.d;
}

QUrl Source::url() const
{
    return d->m_url;
}

Source::DeviceType Source::deviceType() const
{
    return d->m_deviceType;
}

QByteArray Source::deviceName() const
{
    return d->m_deviceName;
}

AbstractMediaStream *Source::stream() const
{
    return d->m_stream;
}

void SourcePrivate::setStream(AbstractMediaStream *s)
{
    m_stream = s;
}

} // namespace Phonon
