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

#include "audiooutput.h"
#include "audiooutput_p.h"

#include "audiooutputadaptor_p.h"
#include "audiooutputinterface.h"
#include "factory_p.h"
#include "globalconfig.h"
#include "objectdescription.h"
#include "phononconfig_p.h"
#include "phononnamespace_p.h"
#include "platform_p.h"
#include "pulsesupport.h"
#ifdef HAVE_PULSEAUDIO
#  include "pulsestream_p.h"
#endif

#include <QtCore/QUuid>
#include <QtCore/qmath.h>

#define PHONON_CLASSNAME AudioOutput
#define IFACES9 AudioOutputInterface49
#define IFECES7 AudioOutputInterface47
#define IFACES2 AudioOutputInterface42
#define IFACES1 IFACES2
#define IFACES0 AudioOutputInterface40, IFACES1, IFECES7, IFACES9
#define PHONON_INTERFACENAME IFACES0

namespace Phonon
{

static inline bool callSetOutputDevice(AudioOutputPrivate *const d, const AudioOutputDevice &dev)
{
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive())
        return pulse->setOutputDevice(d->getStreamUuid(), dev.index());

    if (!d->backendObject())
        return false;

    Iface<IFACES2> iface(d);
    if (iface) {
        return iface->setOutputDevice(dev);
    }
    return Iface<IFACES0>::cast(d)->setOutputDevice(dev.index());
}

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
#ifndef PHONON_NO_DBUS
    adaptor = new AudioOutputAdaptor(q);
    static unsigned int number = 0;
    const QString &path = QLatin1String("/AudioOutputs/") + QString::number(number++);
    QDBusConnection con = QDBusConnection::sessionBus();
    con.registerObject(path, q);
    emit adaptor->newOutputAvailable(con.baseService(), path);
    q->connect(q, SIGNAL(volumeChanged(qreal)), adaptor, SIGNAL(volumeChanged(qreal)));
    q->connect(q, SIGNAL(mutedChanged(bool)), adaptor, SIGNAL(mutedChanged(bool)));
#endif

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

            AudioOutputInterface47 *iface = Iface<AudioOutputInterface47>::cast(this);
            if (iface)
                iface->setStreamUuid(streamUuid);
            else
                pulse->setupStreamEnvironment(streamUuid);
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
    // (cg) Is it possible that PulseAudio initialisation means that the device here is not valid?
    // User reports seem to suggest this possibility but I can't see how :s.
    // See other comment and check for isValid() in handleAutomaticDeviceChange()
    device = AudioOutputDevice::fromIndex(GlobalConfig().audioOutputDeviceFor(category, GlobalConfig::AdvancedDevicesFromSettings | GlobalConfig::HideUnavailableDevices));
    if (m_backendObject) {
        setupBackendObject();
    }
}

QString AudioOutput::name() const
{
    P_D(const AudioOutput);
    return d->name;
}

void AudioOutput::setName(const QString &newName)
{
    P_D(AudioOutput);
    if (d->name == newName) {
        return;
    }
    d->name = newName;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive())
        pulse->setOutputName(d->getStreamUuid(), newName);
    else
        setVolume(Platform::loadVolume(newName));
#ifndef PHONON_NO_DBUS
    if (d->adaptor) {
        emit d->adaptor->nameChanged(newName);
    }
#endif
}

static const qreal LOUDNESS_TO_VOLTAGE_EXPONENT = qreal(0.67);
static const qreal VOLTAGE_TO_LOUDNESS_EXPONENT = qreal(1.0/LOUDNESS_TO_VOLTAGE_EXPONENT);

void AudioOutput::setVolume(qreal volume)
{
    P_D(AudioOutput);
    d->volume = volume;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (k_ptr->backendObject()) {
        if (pulse->isActive()) {
            pulse->setOutputVolume(d->getStreamUuid(), volume);
        } else if (!d->muted) {
            // using Stevens' power law loudness is proportional to (sound pressure)^0.67
            // sound pressure is proportional to voltage:
            // p² \prop P \prop V²
            // => if a factor for loudness of x is requested
            INTERFACE_CALL(setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));
      } else {
          emit volumeChanged(volume);
      }
    } else {
        emit volumeChanged(volume);
    }
    if (!pulse->isActive())
        Platform::saveVolume(d->name, volume);
}

qreal AudioOutput::volume() const
{
    P_D(const AudioOutput);
    if (d->muted || !d->m_backendObject || PulseSupport::getInstance()->isActive())
        return d->volume;
    return pow(INTERFACE_CALL(volume()), LOUDNESS_TO_VOLTAGE_EXPONENT);
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
    return 0.67 * log(INTERFACE_CALL(volume())) / log10over20;
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

    if (d->muted == mute) {
        return;
    }
    d->muted = mute;

    if (!k_ptr->backendObject()) {
        return;
    }

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        pulse->setOutputMute(d->getStreamUuid(), mute);
    } else {
        // When interface 9 is implemented we always default to it.
        Iface<IFACES9> iface9(d);
        if (iface9) {
            iface9->setMuted(mute);
            // iface9 is fully async, we let the backend emit the state change.
            return;
        }

        if (mute) {
            INTERFACE_CALL(setVolume(0.0));
        } else {
            INTERFACE_CALL(setVolume(pow(d->volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));
        }
    }
    emit mutedChanged(mute);
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
    if (k_ptr->backendObject()) {
        return callSetOutputDevice(d, d->device);
    }
    return true;
}

bool AudioOutputPrivate::aboutToDeleteBackendObject()
{
    if (m_backendObject) {
        volume = pINTERFACE_CALL(volume());
    }
    return AbstractAudioOutputPrivate::aboutToDeleteBackendObject();
}

void AudioOutputPrivate::setupBackendObject()
{
    P_Q(AudioOutput);
    Q_ASSERT(m_backendObject);
    AbstractAudioOutputPrivate::setupBackendObject();

    QObject::connect(m_backendObject, SIGNAL(volumeChanged(qreal)), q, SLOT(_k_volumeChanged(qreal)));
    QObject::connect(m_backendObject, SIGNAL(audioDeviceFailed()), q, SLOT(_k_audioDeviceFailed()));
    if (Iface<IFACES9>(this)) {
        QObject::connect(m_backendObject, SIGNAL(mutedChanged(bool)),
                         q, SLOT(_k_mutedChanged(bool)));
    }

    if (!PulseSupport::getInstance()->isActive()) {
        // set up attributes
        pINTERFACE_CALL(setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));

#ifndef QT_NO_PHONON_SETTINGSGROUP
        // if the output device is not available and the device was not explicitly set
        // There is no need to set the output device initially if PA is used as
        // we know it will not work (stream doesn't exist yet) and that this will be
        // handled by _k_deviceChanged()
        if (!callSetOutputDevice(this, device) && !outputDeviceOverridden) {
            // fall back in the preference list of output devices
            QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category, GlobalConfig::AdvancedDevicesFromSettings | GlobalConfig::HideUnavailableDevices);
            if (deviceList.isEmpty()) {
                return;
            }
            for (int i = 0; i < deviceList.count(); ++i) {
                const AudioOutputDevice &dev = AudioOutputDevice::fromIndex(deviceList.at(i));
                if (callSetOutputDevice(this, dev)) {
                    handleAutomaticDeviceChange(dev, AudioOutputPrivate::FallbackChange);
                    return; // found one that works
                }
            }
            // if we get here there is no working output device. Tell the backend.
            const AudioOutputDevice none;
            callSetOutputDevice(this, none);
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
    callSetOutputDevice(this, device);
    P_Q(AudioOutput);
    emit q->outputDeviceChanged(device);
#ifndef PHONON_NO_DBUS
    emit adaptor->outputDeviceIndexChanged(device.index());
#endif
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
            if (callSetOutputDevice(this, info)) {
                handleAutomaticDeviceChange(info, FallbackChange);
                return; // found one that works
            }
        }
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
    // if we get here there is no working output device. Tell the backend.
    const AudioOutputDevice none;
    callSetOutputDevice(this, none);
    handleAutomaticDeviceChange(none, FallbackChange);
}

void AudioOutputPrivate::_k_deviceListChanged()
{
    if (PulseSupport::getInstance()->isActive())
        return;

#ifndef QT_NO_PHONON_SETTINGSGROUP
    pDebug() << Q_FUNC_INFO;
    // Check to see if we have an override and do not change to a higher priority device if the overridden device is still present.
    if (outputDeviceOverridden && device.property("available").toBool()) {
        return;
    }
    // let's see if there's a usable device higher in the preference list
    const QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category, GlobalConfig::AdvancedDevicesFromSettings);
    DeviceChangeType changeType = HigherPreferenceChange;
    for (int i = 0; i < deviceList.count(); ++i) {
        const int devIndex = deviceList.at(i);
        const AudioOutputDevice &info = AudioOutputDevice::fromIndex(devIndex);
        if (!info.property("available").toBool()) {
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
        if (callSetOutputDevice(this, info)) {
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
            if (!callSetOutputDevice(this, device)) {
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
#ifndef PHONON_NO_DBUS
    emit adaptor->outputDeviceIndexChanged(device.index());
#endif
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
            if (device1.property("available").toBool()) {
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

AudioOutputPrivate::~AudioOutputPrivate()
{
    PulseSupport *pulse = PulseSupport::getInstanceOrNull(true);
    if (pulse) {
        pulse->clearStreamCache(streamUuid);
    }
#ifndef PHONON_NO_DBUS
    if (adaptor) {
        emit adaptor->outputDestroyed();
    }
#endif
}

} //namespace Phonon

#include "moc_audiooutput.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME
#undef IFECES7
#undef IFACES2
#undef IFACES1
#undef IFACES0
