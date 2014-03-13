/*
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_SOURCE_P_H_
#define PHONON_SOURCE_P_H_

#include "source.h"

#include <QtCore/QUrl>
#include <QtCore/QPointer>

namespace Phonon {

class PHONON_EXPORT SourcePrivate : public QSharedData
{
public:
    SourcePrivate()
        : m_deviceType(Source::NoDevice)
        , m_stream(0)
        , m_ioDevice(0)
    {
    }

    virtual ~SourcePrivate();

    void setStream(AbstractMediaStream *s);

    QUrl m_url;
    Source::DeviceType m_deviceType;
    QByteArray m_deviceName;

    // The AbstractMediaStream(2) may be deleted at any time by the application. If that happens
    // stream will be 0 automatically, but streamEventQueue will stay valid as we hold a
    // reference to it. This is necessary to avoid a races when setting the MediaSource while
    // another thread deletes the AbstractMediaStream2. StreamInterface(2) will then just get a
    // StreamEventQueue where nobody answers.
    QPointer<AbstractMediaStream> m_stream;
    QIODevice *m_ioDevice;
};

} // namespace Phonon

#endif // PHONON_SOURCE_P_H_
