/*  This file is part of the KDE project
    Copyright (C) 2005-2006, 2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "avcapture.h"
#include "avcaptureinterface.h"
#include "avcapture_p.h"
#include "factory_p.h"
#include "objectdescription.h"

#define PHONON_CLASSNAME AvCapture
#define PHONON_INTERFACENAME AvCaptureInterface

namespace Phonon
{
namespace Experimental
{
PHONON_OBJECT_IMPL

AudioCaptureDevice AvCapture::audioCaptureDevice() const
{
    K_D(const AvCapture);
    int index;
    if (d->m_backendObject)
        index = INTERFACE_CALL(audioCaptureDevice());
    else
        index = d->audioCaptureDevice;
    return AudioCaptureDevice::fromIndex(index);
}

void AvCapture::setAudioCaptureDevice(const AudioCaptureDevice &audioCaptureDevice)
{
    K_D(AvCapture);
    if (d->m_backendObject)
        INTERFACE_CALL(setAudioCaptureDevice(audioCaptureDevice.index()));
    else
        d->audioCaptureDevice = audioCaptureDevice.index();
}

void AvCapture::setAudioCaptureDevice(int audioCaptureDeviceIndex)
{
    K_D(AvCapture);
    if (d->m_backendObject)
        INTERFACE_CALL(setAudioCaptureDevice(audioCaptureDeviceIndex));
    else
        d->audioCaptureDevice = audioCaptureDeviceIndex;
}

VideoCaptureDevice AvCapture::videoCaptureDevice() const
{
    K_D(const AvCapture);
    int index;
    if (d->m_backendObject)
        index = INTERFACE_CALL(videoCaptureDevice());
    else
        index = d->videoCaptureDevice;
    return VideoCaptureDevice::fromIndex(index);
}

void AvCapture::setVideoCaptureDevice(const VideoCaptureDevice &videoCaptureDevice)
{
    K_D(AvCapture);
    if (d->m_backendObject)
        INTERFACE_CALL(setVideoCaptureDevice(videoCaptureDevice.index()));
    else
        d->videoCaptureDevice = videoCaptureDevice.index();
}

void AvCapture::setVideoCaptureDevice(int videoCaptureDeviceIndex)
{
    K_D(AvCapture);
    if (d->m_backendObject)
        INTERFACE_CALL(setVideoCaptureDevice(videoCaptureDeviceIndex));
    else
        d->videoCaptureDevice = videoCaptureDeviceIndex;
}

bool AvCapturePrivate::aboutToDeleteBackendObject()
{
    audioCaptureDevice = pINTERFACE_CALL(audioCaptureDevice());
    videoCaptureDevice = pINTERFACE_CALL(videoCaptureDevice());
    return true;
}

void AvCapturePrivate::setupBackendObject()
{
    Q_ASSERT(m_backendObject);

    // set up attributes
    pINTERFACE_CALL(setAudioCaptureDevice(audioCaptureDevice));
    pINTERFACE_CALL(setVideoCaptureDevice(videoCaptureDevice));
}

} // namespace Experimental
} // namespace Phonon

#include "moc_avcapture.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME
