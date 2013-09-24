/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_BACKENDCAPABILITIES_H
#define PHONON_BACKENDCAPABILITIES_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QObject>

template<class T> class QList;
class QStringList;

namespace Phonon {
namespace BackendCapabilities {

class Notifier : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void capabilitiesChanged();
    void availableAudioOutputDevicesChanged();
    void availableAudioCaptureDevicesChanged();
    void availableVideoCaptureDevicesChanged();
};

PHONON_EXPORT Notifier *notifier();

PHONON_EXPORT QList<AudioOutputDevice> availableAudioOutputDevices();
PHONON_EXPORT QList<AudioCaptureDevice> availableAudioCaptureDevices();
PHONON_EXPORT QList<VideoCaptureDevice> availableVideoCaptureDevices();
PHONON_EXPORT QList<VideoCaptureDevice> availableAVCaptureDevices();

} // namespace BackendCapabilities
} // namespace Phonon

#endif // PHONON_BACKENDCAPABILITIES_H
