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
#include "globalconfig.h"
#include "objectdescription.h"
#include "phononnamespace_p.h"
#include "pulsesupport.h"
#ifdef HAVE_PULSEAUDIO
#  include "pulsestream_p.h"
#endif

#include <QtCore/QUuid>
#include <QtCore/qmath.h>

namespace Phonon {

AudioOutput::AudioOutput(Phonon::Category category, QObject *parent)
    : AbstractAudioOutput(*new AudioOutputPrivate, parent)
{
    P_D(AudioOutput);
    d->init(category);
}

AudioOutput::AudioOutput(QObject *parent)
    : AbstractAudioOutput(*new AudioOutputPrivate, parent)
{
    P_D(AudioOutput);
    d->init(NoCategory);
}

void AudioOutputPrivate::init(Phonon::Category c)
{
    P_Q(AudioOutput);

    category = c;
#ifndef QT_NO_QUUID_STRING
    streamUuid = QUuid::createUuid().toString();
#endif

    createBackendObject();

#ifdef HAVE_PULSEAUDIO
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        PulseStream *stream = pulse->registerOutputStream(streamUuid, category);
        if (stream) {
            q->connect(stream, SIGNAL(usingDevice(int)), SLOT(_k_deviceChanged(int)));
            q->connect(stream, SIGNAL(volumeChanged(qreal)), SLOT(_k_volumeChanged(qreal)));
            q->connect(stream, SIGNAL(muteChanged(bool)), SLOT(_k_mutedChanged(bool)));
        }
    }
#endif

    q->connect(Factory::sender(), SIGNAL(availableAudioOutputDevicesChanged()), SLOT(_k_deviceListChanged()));
}

QString AudioOutputPrivate::getStreamUuid()
{
    return streamUuid;
}

void AudioOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    P_Q(AudioOutput);
    m_backendObject = Factory::createAudioOutput(q);
    interface = qobject_cast<AudioOutputInterface *>(m_backendObject);
    // (cg) Is it possible that PulseAudio initialisation means that the device here is not valid?
    // User reports seem to suggest this possibility but I can't see how :s.
    // See other comment and check for isValid() in handleAutomaticDeviceChange()
    device = AudioOutputDevice::fromIndex(GlobalConfig().audioOutputDeviceFor(category, GlobalConfig::AdvancedDevicesFromSettings | GlobalConfig::HideUnavailableDevices));
    if (m_backendObject && interface) {
        setupBackendObject();
    }
}

static const qreal LOUDNESS_TO_VOLTAGE_EXPONENT = qreal(0.67);
static const qreal VOLTAGE_TO_LOUDNESS_EXPONENT = qreal(1.0/LOUDNESS_TO_VOLTAGE_EXPONENT);

void AudioOutput::setVolume(qreal volume)
{
    P_D(AudioOutput);
    d->volume = volume;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (d->backendObject()) {
        if (pulse->isActive()) {
            pulse->setOutputVolume(d->getStreamUuid(), volume);
        } else if (!d->muted && d->interface) {
            // using Stevens' power law loudness is proportional to (sound pressure)^0.67
            // sound pressure is proportional to voltage:
            // p² \prop P \prop V²
            // => if a factor for loudness of x is requested
            d->interface->setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT));
      } else {
          emit volumeChanged(volume);
      }
    } else {
        emit volumeChanged(volume);
    }
}

qreal AudioOutput::volume() const
{
    P_D(const AudioOutput);
    if (d->muted || !d->interface || PulseSupport::getInstance()->isActive())
        return d->volume;
    return pow(d->interface->volume(), LOUDNESS_TO_VOLTAGE_EXPONENT);
}

#ifndef PHONON_LOG10OVER20
#define PHONON_LOG10OVER20
static const qreal log10over20 = qreal(0.1151292546497022842); // ln(10) / 20
#endif // PHONON_LOG10OVER20

qreal AudioOutput::volumeDecibel() const
{
    P_D(const AudioOutput);
    if (d->muted || !d->m_backendObject || PulseSupport::getInstance()->isActive())
        return log(d->volume) / log10over20;
    return 0.67 * log(d->interface->volume()) / log10over20;
}

void AudioOutput::setVolumeDecibel(qreal newVolumeDecibel)
{
    setVolume(exp(newVolumeDecibel * log10over20));
}

bool AudioOutput::isMuted() const
{
    P_D(const AudioOutput);
    return d->muted;
}

void AudioOutput::setMuted(bool mute)
{
    P_D(AudioOutput);
    if (d->muted != mute) {
        PulseSupport *pulse = PulseSupport::getInstance();
        if (mute) {
            d->muted = mute;
            if (k_ptr->backendObject()) {
                if (pulse->isActive())
                    pulse->setOutputMute(d->getStreamUuid(), mute);
                else
                    d->interface->setVolume(0.0);
            }
        } else {
            if (k_ptr->backendObject()) {
                if (pulse->isActive())
                    pulse->setOutputMute(d->getStreamUuid(), mute);
                else
                    d->interface->setVolume(pow(d->volume, VOLTAGE_TO_LOUDNESS_EXPONENT));
            }
            d->muted = mute;
        }
        emit mutedChanged(mute);
    }
}

Category AudioOutput::category() const
{
    P_D(const AudioOutput);
    return d->category;
}

AudioOutputDevice AudioOutput::outputDevice() const
{
    P_D(const AudioOutput);
    return d->device;
}

bool AudioOutput::setOutputDevice(const AudioOutputDevice &newAudioOutputDevice)
{
    P_D(AudioOutput);
    if (!newAudioOutputDevice.isValid()) {
        d->outputDeviceOverridden = d->forceMove = false;
        const int newIndex = GlobalConfig().audioOutputDeviceFor(d->category);
        if (newIndex == d->device.index()) {
            return true;
        }
        d->device = AudioOutputDevice::fromIndex(newIndex);
    } else {
        d->outputDeviceOverridden = d->forceMove = true;
        if (d->device == newAudioOutputDevice) {
            return true;
        }
        d->device = newAudioOutputDevice;
    }
    if (d->backendObject()) {
        return d->callSetOutputDevice(d->device);
    }
    return true;
}

bool AudioOutputPrivate::aboutToDeleteBackendObject()
{
    if (interface)
        volume = interface->volume();
    return AbstractAudioOutputPrivate::aboutToDeleteBackendObject();
}

void AudioOutputPrivate::setupBackendObject()
{
    P_Q(AudioOutput);
    Q_ASSERT(m_backendObject);
    AbstractAudioOutputPrivate::setupBackendObject();

    QObject::connect(m_backendObject, SIGNAL(volumeChanged(qreal)), q, SLOT(_k_volumeChanged(qreal)));
    QObject::connect(m_backendObject, SIGNAL(audioDeviceFailed()), q, SLOT(_k_audioDeviceFailed()));

    if (!PulseSupport::getInstance()->isActive()) {
        // set up attributes
        interface->setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT));

#ifndef QT_NO_PHONON_SETTINGSGROUP
        // if the output device is not available and the device was not explicitly set
        // There is no need to set the output device initially if PA is used as
        // we know it will not work (stream doesn't exist yet) and that this will be
        // handled by _k_deviceChanged()
        if (!callSetOutputDevice(device) && !outputDeviceOverridden) {
            // fall back in the preference list of output devices
            QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category, GlobalConfig::AdvancedDevicesFromSettings | GlobalConfig::HideUnavailableDevices);
            if (deviceList.isEmpty()) {
                return;
            }
            for (int i = 0; i < deviceList.count(); ++i) {
                const AudioOutputDevice &dev = AudioOutputDevice::fromIndex(deviceList.at(i));
                if (callSetOutputDevice(dev)) {
                    handleAutomaticDeviceChange(dev, AudioOutputPrivate::FallbackChange);
                    return; // found one that works
                }
            }
            // if we get here there is no working output device. Tell the backend.
            const AudioOutputDevice none;
            callSetOutputDevice(none);
            handleAutomaticDeviceChange(none, FallbackChange);
        }
#endif //QT_NO_PHONON_SETTINGSGROUP
    }
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

void AudioOutputPrivate::_k_revertFallback()
{
    if (deviceBeforeFallback == -1) {
        return;
    }
    device = AudioOutputDevice::fromIndex(deviceBeforeFallback);
    callSetOutputDevice(device);
    P_Q(AudioOutput);
    emit q->outputDeviceChanged(device);
}

void AudioOutputPrivate::_k_audioDeviceFailed()
{
    if (PulseSupport::getInstance()->isActive())
        return;

#ifndef QT_NO_PHONON_SETTINGSGROUP

    pDebug() << Q_FUNC_INFO;
    // outputDeviceIndex identifies a failing device
    // fall back in the preference list of output devices
    const QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category, GlobalConfig::AdvancedDevicesFromSettings | GlobalConfig::HideUnavailableDevices);
    for (int i = 0; i < deviceList.count(); ++i) {
        const int devIndex = deviceList.at(i);
        // if it's the same device as the one that failed, ignore it
        if (device.index() != devIndex) {
            const AudioOutputDevice &info = AudioOutputDevice::fromIndex(devIndex);
            if (callSetOutputDevice(info)) {
                handleAutomaticDeviceChange(info, FallbackChange);
                return; // found one that works
            }
        }
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
    // if we get here there is no working output device. Tell the backend.
    const AudioOutputDevice none;
    callSetOutputDevice(none);
    handleAutomaticDeviceChange(none, FallbackChange);
}

void AudioOutputPrivate::_k_deviceListChanged()
{
    if (PulseSupport::getInstance()->isActive())
        return;

#ifndef QT_NO_PHONON_SETTINGSGROUP
    pDebug() << Q_FUNC_INFO;
    // Check to see if we have an override and do not change to a higher priority device if the overridden device is still present.
    if (outputDeviceOverridden && device.isAvailable()) {
        return;
    }
    // let's see if there's a usable device higher in the preference list
    const QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category, GlobalConfig::AdvancedDevicesFromSettings);
    DeviceChangeType changeType = HigherPreferenceChange;
    for (int i = 0; i < deviceList.count(); ++i) {
        const int devIndex = deviceList.at(i);
        const AudioOutputDevice &info = AudioOutputDevice::fromIndex(devIndex);
        if (!info.isAvailable()) {
            if (device.index() == devIndex) {
                // we've reached the currently used device and it's not available anymore, so we
                // fallback to the next available device
                changeType = FallbackChange;
            }
            pDebug() << devIndex << "is not available";
            continue;
        }
        pDebug() << devIndex << "is available";
        if (device.index() == devIndex) {
            // we've reached the currently used device, nothing to change
            break;
        }
        if (callSetOutputDevice(info)) {
            handleAutomaticDeviceChange(info, changeType);
            break; // found one with higher preference that works
        }
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
}

void AudioOutputPrivate::_k_deviceChanged(int deviceIndex)
{
    // NB that this method is only used by PulseAudio at present.

    // 1. Check to see if we are overridden. If we are, and devices do not match,
    //    then try and apply our own device as the output device.
    //    We only do this the first time
    if (outputDeviceOverridden && forceMove) {
        forceMove = false;
        const AudioOutputDevice &currentDevice = AudioOutputDevice::fromIndex(deviceIndex);
        if (currentDevice != device) {
            if (!callSetOutputDevice(device)) {
                // What to do if we are overridden and cannot change to our preferred device?
            }
        }
    }
    // 2. If we are not overridden, then we need to update our perception of what
    //    device we are using. If the devices do not match, something lower in the
    //    stack is overriding our preferences (e.g. a per-application stream preference,
    //    specific application move, priority list changed etc. etc.)
    else if (!outputDeviceOverridden) {
        const AudioOutputDevice &currentDevice = AudioOutputDevice::fromIndex(deviceIndex);
        if (currentDevice != device) {
            // The device is not what we think it is, so lets say what is happening.
            handleAutomaticDeviceChange(currentDevice, SoundSystemChange);
        }
    }
}

    static struct
    {
        int first;
        int second;
    } g_lastFallback = { 0, 0 };

void AudioOutputPrivate::handleAutomaticDeviceChange(const AudioOutputDevice &device2, DeviceChangeType type)
{
    P_Q(AudioOutput);
    deviceBeforeFallback = device.index();
    device = device2;
    emit q->outputDeviceChanged(device2);
    const AudioOutputDevice &device1 = AudioOutputDevice::fromIndex(deviceBeforeFallback);
    switch (type) {
    case FallbackChange:
        if (g_lastFallback.first != device1.index() || g_lastFallback.second != device2.index()) {
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
            const QString &text = //device2.isValid() ?
                AudioOutput::tr("<html>The audio playback device <b>%1</b> does not work.<br/>"
                        "Falling back to <b>%2</b>.</html>").arg(device1.name()).arg(device2.name()) /*:
                AudioOutput::tr("<html>The audio playback device <b>%1</b> does not work.<br/>"
                        "No other device available.</html>").arg(device1.name())*/;
            Platform::notification("AudioDeviceFallback", text);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
            g_lastFallback.first = device1.index();
            g_lastFallback.second = device2.index();
        }
        break;
    case HigherPreferenceChange:
        {
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        const QString text = AudioOutput::tr("<html>Switching to the audio playback device <b>%1</b><br/>"
                "which just became available and has higher preference.</html>").arg(device2.name());
        Platform::notification("AudioDeviceFallback", text,
                QStringList(AudioOutput::tr("Revert back to device '%1'").arg(device1.name())),
                q, SLOT(_k_revertFallback()));
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        g_lastFallback.first = 0;
        g_lastFallback.second = 0;
        }
        break;
    case SoundSystemChange:
        {
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        // If device1 is not "valid" this indicates that the preferences used to select
        // a device was perhaps not available when this object was created (although
        // I can't quite work out how that would be....)
        if (device1.isValid()) {
            if (device1.isAvailable()) {
                const QString text = AudioOutput::tr("<html>Switching to the audio playback device <b>%1</b><br/>"
                        "which has higher preference or is specifically configured for this stream.</html>").arg(device2.name());
                Platform::notification("AudioDeviceFallback", text,
                        QStringList(AudioOutput::tr("Revert back to device '%1'").arg(device1.name())),
                        q, SLOT(_k_revertFallback()));
            } else {
                const QString &text =
                    AudioOutput::tr("<html>The audio playback device <b>%1</b> does not work.<br/>"
                            "Falling back to <b>%2</b>.</html>").arg(device1.name()).arg(device2.name());
                Platform::notification("AudioDeviceFallback", text);
            }
        }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        //outputDeviceOverridden = true;
        g_lastFallback.first = 0;
        g_lastFallback.second = 0;
        }
        break;
    }
}

bool AudioOutputPrivate::callSetOutputDevice(const AudioOutputDevice &dev)
{
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive())
        return pulse->setOutputDevice(streamUuid, dev.index());

    if (!interface)
        return false;
    return interface->setOutputDevice(dev);
}

AudioOutputPrivate::~AudioOutputPrivate()
{
    PulseSupport::getInstance()->clearStreamCache(streamUuid);
}

} //namespace Phonon

#include "moc_audiooutput.cpp"
