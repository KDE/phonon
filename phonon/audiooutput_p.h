/*
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_AUDIOOUTPUT_P_H
#define PHONON_AUDIOOUTPUT_P_H

#include "abstractoutput_p.h"
#include "audiooutput.h"

namespace Phonon {

class AudioOutputInterface;

class AudioOutputPrivate : public AbstractOutputPrivate
{
protected:
    AudioOutputPrivate()
        : AbstractOutputPrivate()
        , interface(0)
        , volume(1.0)
        , deviceBeforeFallback(-1)
        , outputDeviceOverridden(false)
        , forceMove(false)
        , muted(false)
    {
    }

    /** \reimp */
    virtual void createBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

    void setupBackendObject();

    void init(Phonon::Category c);

    enum DeviceChangeType {
        FallbackChange,
        HigherPreferenceChange,
        SoundSystemChange
    };
    void handleAutomaticDeviceChange(const AudioOutputDevice &newDev, DeviceChangeType type);

    /**
     * \brief Checks if interface is null and sets output device.
     * \param dev Device to set
     * \return \c false if interface==0
     */
    bool safeSetOutputDevice(const AudioOutputDevice &dev);

    void _k_volumeChanged(qreal);
    void _k_mutedChanged(bool);
    void _k_revertFallback();
    void _k_audioDeviceFailed();
    void _k_deviceListChanged();
    void _k_deviceChanged(int deviceIndex);

private:
    AudioOutputInterface *interface;
    qreal volume;
    Category category;
    int deviceBeforeFallback;
    bool outputDeviceOverridden;
    bool forceMove;
    bool muted;

    P_DECLARE_PUBLIC(AudioOutput)
};

} //namespace Phonon

#endif // PHONON_AUDIOOUTPUT_P_H
