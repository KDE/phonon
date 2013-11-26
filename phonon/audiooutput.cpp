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

#include "audiooutput.h"
#include "audiooutput_p.h"
#include "audiooutputinterface.h"

#include "factory_p.h"
#include "objectdescription.h"
#include "phononnamespace_p.h"

#include <QtCore/QUuid>
#include <QtCore/qmath.h>

namespace Phonon {

AudioOutput::AudioOutput(Phonon::Category category, QObject *parent)
    : QObject(parent)
    , AbstractOutput(*new AudioOutputPrivate)
{
    P_D(AudioOutput);
    d->init(category);
}

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent)
    , AbstractOutput(*new AudioOutputPrivate)
{
    P_D(AudioOutput);
    d->init(NoCategory);
}

void AudioOutputPrivate::init(Phonon::Category c)
{
    P_Q(AudioOutput);
    category = c;
    createBackendObject();
}

void AudioOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    P_Q(AudioOutput);
    m_backendObject = Factory::createAudioOutput(q);
    interface = qobject_cast<AudioOutputInterface *>(m_backendObject);
    if (m_backendObject && interface)
        setupBackendObject();
}

static const qreal LOUDNESS_TO_VOLTAGE_EXPONENT = qreal(0.67);
static const qreal VOLTAGE_TO_LOUDNESS_EXPONENT = qreal(1.0/LOUDNESS_TO_VOLTAGE_EXPONENT);

void AudioOutput::setVolume(qreal volume)
{
    P_D(AudioOutput);
    d->volume = volume;

    if (d->interface) { // Fake property change.
        emit volumeChanged(volume);
        return;
    }

    if (!d->muted) {
        // using Stevens' power law loudness is proportional to (sound pressure)^0.67
        // sound pressure is proportional to voltage:
        // p² \prop P \prop V²
        // => if a factor for loudness of x is requested
        d->interface->setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT));
    }
}

qreal AudioOutput::volume() const
{
    P_D(const AudioOutput);
    if (d->muted || !d->interface)
        return d->volume;
#warning why
    return pow(d->interface->volume(), LOUDNESS_TO_VOLTAGE_EXPONENT);
}

bool AudioOutput::isMuted() const
{
    P_D(const AudioOutput);
    return d->muted;
}

void AudioOutput::setMuted(bool mute)
{
    P_D(AudioOutput);
    if (d->muted == mute)
        return;
    d->muted = mute;

    qreal volume = 0.0;
#warning it may be a good idea to restore the volume via the backend....
    if (!mute) // Restore volume.
        volume = pow(d->volume, VOLTAGE_TO_LOUDNESS_EXPONENT);

    if (d->interface)
#warning how would that emit mutedChanged? if (vol==0) ???
        d->interface->setVolume(volume);
    else // Fake mute.
        emit mutedChanged(mute);
}

Category AudioOutput::category() const
{
    P_D(const AudioOutput);
    return d->category;
}

AudioOutputDevice AudioOutput::outputDevice() const
{
#warning no backing
    return AudioOutputDevice();
}

bool AudioOutput::setOutputDevice(const AudioOutputDevice &newAudioOutputDevice)
{
#warning nop backing
    return false;
}

void AudioOutputPrivate::setupBackendObject()
{
    P_Q(AudioOutput);
    Q_ASSERT(m_backendObject);

    QObject::connect(m_backendObject, SIGNAL(volumeChanged(qreal)), q, SLOT(_k_volumeChanged(qreal)));

    // set up attributes
    interface->setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT));
}

void AudioOutputPrivate::_k_volumeChanged(qreal newVolume)
{
    volume = pow(newVolume, LOUDNESS_TO_VOLTAGE_EXPONENT);
    if (!muted) {
        P_Q(AudioOutput);
        emit q->volumeChanged(volume);
    }
}

void AudioOutputPrivate::_k_mutedChanged(bool newMuted)
{
    muted = newMuted;
    P_Q(AudioOutput);
    emit q->mutedChanged(newMuted);
}

} // namespace Phonon

#include "moc_audiooutput.cpp"
