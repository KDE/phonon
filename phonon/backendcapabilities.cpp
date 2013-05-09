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

#include "backendcapabilities.h"
#include "backendcapabilities_p.h"

#include "phonondefs_p.h"
#include "backendinterface.h"
#include "factory_p.h"
#include "globalconfig.h"
#include "globalstatic_p.h"
#include "objectdescription.h"

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStringList>

PHONON_GLOBAL_STATIC(Phonon::BackendCapabilitiesPrivate, globalBCPrivate)

namespace Phonon {

BackendCapabilities::Notifier *BackendCapabilities::notifier()
{
    return globalBCPrivate;
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevices()
{
    QList<AudioOutputDevice> ret;
#ifndef QT_NO_PHONON_SETTINGSGROUP
    const QList<int> deviceIndexes = GlobalConfig().audioOutputDeviceListFor(Phonon::NoCategory, GlobalConfig::ShowAdvancedDevices);
    for (int i = 0; i < deviceIndexes.count(); ++i) {
        ret.append(AudioOutputDevice::fromIndex(deviceIndexes.at(i)));
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
    return ret;
}


#ifndef PHONON_NO_AUDIOCAPTURE
QList<AudioCaptureDevice> BackendCapabilities::availableAudioCaptureDevices()
{
    QList<AudioCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioCaptureDeviceListFor(Phonon::NoCaptureCategory, GlobalConfig::ShowAdvancedDevices);
    for (int i = 0; i < deviceIndexes.count(); ++i) {
        ret.append(AudioCaptureDevice::fromIndex(deviceIndexes.at(i)));
    }
    return ret;
}
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
QList<VideoCaptureDevice> BackendCapabilities::availableVideoCaptureDevices()
{
    QList<VideoCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().videoCaptureDeviceListFor(Phonon::NoCaptureCategory, GlobalConfig::ShowAdvancedDevices);
    for (int i = 0; i < deviceIndexes.count(); ++i) {
        ret.append(VideoCaptureDevice::fromIndex(deviceIndexes.at(i)));
    }
    return ret;
}
#endif //PHONON_NO_VIDEOCAPTURE

#if !defined(PHONON_NO_VIDEOCAPTURE) && !defined(PHONON_NO_AUDIOCAPTURE)
QList<VideoCaptureDevice> BackendCapabilities::availableAVCaptureDevices()
{
    QList<VideoCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().videoCaptureDeviceListFor(Phonon::NoCaptureCategory, GlobalConfig::ShowAdvancedDevices);
    for (int i = 0; i < deviceIndexes.count(); ++i) {
        VideoCaptureDevice vcd = VideoCaptureDevice::fromIndex(deviceIndexes.at(i));
        if (vcd.propertyNames().contains("hasaudio") && vcd.property("hasaudio").isValid())
            ret.append(vcd);
    }
    return ret;
}
#endif // NOT PHONON_NO_VIDEOCAPTURE AND NOT PHONON_NO_AUDIOCAPTURE

} // namespace Phonon

#include "moc_backendcapabilities.cpp"
