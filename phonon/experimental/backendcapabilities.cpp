/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), Nokia Corporation (or its successors, 
    if any) and the KDE Free Qt Foundation, which shall act as a proxy 
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "backendcapabilities.h"
#include "globalconfig_p.h"

namespace Phonon
{
namespace Experimental
{

QList<VideoCaptureDevice> BackendCapabilities::availableVideoCaptureDevices()
{
    QList<VideoCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().videoCaptureDeviceListFor(Phonon::NoCategory);
    foreach (int i, deviceIndexes) {
        ret.append(VideoCaptureDevice::fromIndex(i));
    }
    return ret;
}

bool BackendCapabilities::getHideAdvancedDevices()
{
    return GlobalConfig().getHideAdvancedDevices();
}

void BackendCapabilities::hideAdvancedDevices(bool hide)
{
    GlobalConfig().hideAdvancedDevices(hide);
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevicesForCategory(Phonon::Category category)
{
    QList<AudioOutputDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioOutputDeviceListFor(category);
    foreach (int i, deviceIndexes) {
        ret.append(AudioOutputDevice::fromIndex(i));
    }
    return ret;
}

QList<AudioCaptureDevice> BackendCapabilities::availableAudioCaptureDevicesForCategory(Phonon::Category category)
{
    QList<AudioCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioCaptureDeviceListFor(category);
    foreach (int i, deviceIndexes) {
        ret.append(AudioCaptureDevice::fromIndex(i));
    }
    return ret;
}

void BackendCapabilities::setAudioOutputDevicePriorityListForCategory(Phonon::Category category, QList<int> devices)
{
    GlobalConfig().setAudioOutputDeviceListFor(category, devices);
}

void BackendCapabilities::setAudioCaptureDevicePriorityListForCategory(Phonon::Category category, QList<int> devices)
{
    GlobalConfig().setAudioCaptureDeviceListFor(category, devices);
}

} // namespace Experimental
} // namespace Phonon
