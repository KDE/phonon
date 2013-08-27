/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef AUDIOOUTPUT_P_H
#define AUDIOOUTPUT_P_H

#include "abstractaudiooutput_p.h"
#include "audiooutput.h"
#include "audiooutputinterface.h"
#include "platform_p.h"

namespace Phonon {

class AudioOutputAdaptor;

class AudioOutputPrivate : public AbstractAudioOutputPrivate
{
public:
    void init(Phonon::Category c);
    QString getStreamUuid();

protected:
    AudioOutputPrivate()
        : AbstractAudioOutputPrivate()
        , interface(0)
        , volume(1.0)
        , deviceBeforeFallback(-1)
        , outputDeviceOverridden(false)
        , forceMove(false)
        , muted(false)
    {
    }

    ~AudioOutputPrivate();

    /** \reimp */
    virtual bool aboutToDeleteBackendObject();

    /** \reimp */
    virtual void createBackendObject();

    /** \reimp */
    void setupBackendObject();

    enum DeviceChangeType {
        FallbackChange,
        HigherPreferenceChange,
        SoundSystemChange
    };
    void handleAutomaticDeviceChange(const AudioOutputDevice &newDev, DeviceChangeType type);

    bool callSetOutputDevice(const AudioOutputDevice &dev);

    void _k_volumeChanged(qreal);
    void _k_mutedChanged(bool);
    void _k_revertFallback();
    void _k_audioDeviceFailed();
    void _k_deviceListChanged();
    void _k_deviceChanged(int deviceIndex);

private:
    AudioOutputInterface *interface;
    Phonon::AudioOutputDevice device;
    qreal volume;
    QString streamUuid;
    Category category;
    int deviceBeforeFallback;
    bool outputDeviceOverridden;
    bool forceMove;
    bool muted;

    P_DECLARE_PUBLIC(AudioOutput)
};

} //namespace Phonon

#endif // AUDIOOUTPUT_P_H
