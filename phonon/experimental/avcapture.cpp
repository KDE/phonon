/*  This file is part of the KDE project
    Copyright (C) 2005-2006, 2008 Matthias Kretz <kretz@kde.org>

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

#include "phononnamespace_p.h"

#if defined(PHONON_NO_VIDEOCAPTURE) || defined(PHONON_NO_AUDIOCAPTURE)
#define NO_PHONON_AVCAPTURE
#endif

#ifndef NO_PHONON_AVCAPTURE

#include "avcapture.h"
#include "avcapture_p.h"

#include "avcaptureinterface.h"
#include "factory_p.h"
#include "globalconfig.h"

#define PHONON_CLASSNAME AvCapture
#define PHONON_INTERFACENAME AvCaptureInterface

namespace Phonon
{
namespace Experimental
{
PHONON_OBJECT_IMPL

AvCapture::AvCapture(Phonon::CaptureCategory category, QObject *parent)
    : QObject(parent), MediaNode(*new AvCapturePrivate())
{
    setCaptureDevices(category);
}

Phonon::State AvCapture::state() const
{
    P_D(const AvCapture);
    if (d->m_backendObject) {
        return INTERFACE_CALL(state());
    }
    return Phonon::StoppedState;
}

void AvCapture::start()
{
    P_D(AvCapture);
    if (d->backendObject()) {
        INTERFACE_CALL(start());
    }
}

void AvCapture::pause()
{
    P_D(AvCapture);
    if (d->backendObject()) {
        INTERFACE_CALL(pause());
    }
}

void AvCapture::stop()
{
    P_D(AvCapture);
    if (d->backendObject()) {
        INTERFACE_CALL(stop());
    }
}

void AvCapture::setCaptureDevices(Phonon::CaptureCategory category)
{
    setAudioCaptureDevice(category);
    setVideoCaptureDevice(category);
}

Phonon::AudioCaptureDevice AvCapture::audioCaptureDevice() const
{
    P_D(const AvCapture);
    if (d->m_backendObject) {
        return INTERFACE_CALL(audioCaptureDevice());
    }
    return d->audioCaptureDevice;
}

void AvCapture::setAudioCaptureDevice(const Phonon::AudioCaptureDevice &audioCaptureDevice)
{
    P_D(AvCapture);
    d->audioCaptureDevice = audioCaptureDevice;
    if (d->m_backendObject) {
        INTERFACE_CALL(setAudioCaptureDevice(d->audioCaptureDevice));
    }
}

void AvCapture::setAudioCaptureDevice(Phonon::CaptureCategory category)
{
    P_D(AvCapture);
    d->audioCaptureDevice = AudioCaptureDevice::fromIndex(Phonon::GlobalConfig().audioCaptureDeviceFor(category));
    if (d->m_backendObject) {
        INTERFACE_CALL(setAudioCaptureDevice(d->audioCaptureDevice));
    }
}

PHONON_DEPRECATED void AvCapture::setAudioCaptureDevice(Phonon::Category category)
{
    setAudioCaptureDevice(Phonon::categoryToCaptureCategory(category));
}

Phonon::VideoCaptureDevice AvCapture::videoCaptureDevice() const
{
    P_D(const AvCapture);
    if (d->m_backendObject) {
        return INTERFACE_CALL(videoCaptureDevice());
    }
    return d->videoCaptureDevice;
}

void AvCapture::setVideoCaptureDevice(const Phonon::Experimental::VideoCaptureDevice &videoCaptureDevice)
{
    setVideoCaptureDevice(phononExperimentalVcdToVcd(videoCaptureDevice));
}

void AvCapture::setVideoCaptureDevice(const Phonon::VideoCaptureDevice &videoCaptureDevice)
{
    P_D(AvCapture);
    d->videoCaptureDevice = videoCaptureDevice;
    if (d->m_backendObject) {
        INTERFACE_CALL(setVideoCaptureDevice(d->videoCaptureDevice));
    }
}

void AvCapture::setVideoCaptureDevice(Phonon::CaptureCategory category)
{
    P_D(AvCapture);
    d->videoCaptureDevice = Phonon::VideoCaptureDevice::fromIndex(Phonon::GlobalConfig().videoCaptureDeviceFor(category));
    if (d->m_backendObject) {
        INTERFACE_CALL(setVideoCaptureDevice(d->videoCaptureDevice));
    }
}

void AvCapture::setVideoCaptureDevice(Phonon::Category category)
{
    setVideoCaptureDevice(Phonon::categoryToCaptureCategory(category));
}

bool AvCapturePrivate::aboutToDeleteBackendObject()
{
    audioCaptureDevice = pINTERFACE_CALL(audioCaptureDevice());
    videoCaptureDevice = pINTERFACE_CALL(videoCaptureDevice());
    return true;
}

void AvCapturePrivate::setupBackendObject()
{
    P_Q(AvCapture);
    Q_ASSERT(m_backendObject);

    QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), q, SIGNAL(stateChanged(Phonon::State,Phonon::State)), Qt::QueuedConnection);

    // set up attributes
    pINTERFACE_CALL(setAudioCaptureDevice(audioCaptureDevice));
    pINTERFACE_CALL(setVideoCaptureDevice(videoCaptureDevice));
}

} // namespace Experimental
} // namespace Phonon

#include "moc_avcapture.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME

#endif // NO_PHONON_AVCAPTURE
