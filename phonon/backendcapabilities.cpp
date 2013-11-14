/*
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>
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

#include "backendcapabilities.h"

#include "phonondefs_p.h"
#include "backendinterface.h"
#include "factory_p.h"
#include "globalstatic_p.h"
#include "objectdescription.h"

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStringList>

namespace Phonon {

BackendCapabilities::Notifier::Notifier(QObject *parent)
    : QObject(parent)
{
    connect(Factory::sender(), SIGNAL(availableAudioOutputDevicesChanged()),
            this, SIGNAL(availableAudioOutputDevicesChanged()));
    connect(Factory::sender(), SIGNAL(availableAudioCaptureDevicesChanged()),
            this, SIGNAL(availableAudioCaptureDevicesChanged()));
    connect(Factory::sender(), SIGNAL(availableVideoCaptureDevicesChanged()),
            this, SIGNAL(availableVideoCaptureDevicesChanged()));
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevices()
{
    return qobject_cast<BackendInterface *>(Factory::backend())->audioOutputDevices();
}

QList<AudioCaptureDevice> BackendCapabilities::availableAudioCaptureDevices()
{
#warning no backing
    return QList<AudioCaptureDevice>();
}

QList<VideoCaptureDevice> BackendCapabilities::availableVideoCaptureDevices()
{
#warning no backing
    return QList<VideoCaptureDevice>();
}

QList<VideoCaptureDevice> BackendCapabilities::availableAVCaptureDevices()
{
#warning no backing
    return QList<VideoCaptureDevice>();
}

} // namespace Phonon

#include "moc_backendcapabilities.cpp"
