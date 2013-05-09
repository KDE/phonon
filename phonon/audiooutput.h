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

#ifndef PHONON_AUDIOOUTPUT_H
#define PHONON_AUDIOOUTPUT_H

#include "phonon_export.h"
#include "abstractaudiooutput.h"
#include "phononnamespace.h"
#include "objectdescription.h"

class QString;

namespace Phonon {

class AudioOutputPrivate;

class PHONON_EXPORT AudioOutput : public AbstractAudioOutput
{
    friend class FactoryPrivate;
    Q_OBJECT
public:
    explicit AudioOutput(Phonon::Category category, QObject *parent = 0);
    explicit AudioOutput(QObject *parent = 0);
    qreal volume() const;
    qreal volumeDecibel() const;
    Phonon::Category category() const;
    AudioOutputDevice outputDevice() const;
    bool isMuted() const;

public Q_SLOTS:
    void setVolume(qreal newVolume);
    void setVolumeDecibel(qreal newVolumeDecibel);
    bool setOutputDevice(const Phonon::AudioOutputDevice &newAudioOutputDevice);
    void setMuted(bool mute);

Q_SIGNALS:
    void volumeChanged(qreal newVolume);
    void mutedChanged(bool);
    void outputDeviceChanged(const Phonon::AudioOutputDevice &newAudioOutputDevice);

private:
    P_DECLARE_PRIVATE(AudioOutput)
    Q_PRIVATE_SLOT(k_func(), void _k_volumeChanged(qreal))
    Q_PRIVATE_SLOT(k_func(), void _k_mutedChanged(bool))
    Q_PRIVATE_SLOT(k_func(), void _k_revertFallback())
    Q_PRIVATE_SLOT(k_func(), void _k_audioDeviceFailed())
    Q_PRIVATE_SLOT(k_func(), void _k_deviceListChanged())
    Q_PRIVATE_SLOT(k_func(), void _k_deviceChanged(int device))
};

} // namespace Phonon

#endif // PHONON_AUDIOOUTPUT_H
