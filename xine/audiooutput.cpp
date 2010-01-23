/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "audiooutput.h"
#include <QVector>
#include <QtCore/QCoreApplication>

#include <sys/ioctl.h>
#include <iostream>
#include <QSet>
#include <phonon/pulsesupport.h>
#include "mediaobject.h"
#include "backend.h"
#include "events.h"
#include "wirecall.h"
#include "xineengine.h"
#include "xinethread.h"
#include "keepreference.h"
#include "audiodataoutput.h"

#include <xine/audio_out.h>

// the gcc 4.0 STL includes assert.h
#undef assert

namespace Phonon
{
namespace Xine
{

AudioOutput::AudioOutput(QObject *parent)
    : AbstractAudioOutput(new AudioOutputXT, parent)
{
}

AudioOutput::~AudioOutput()
{
    //debug() << Q_FUNC_INFO ;
}

AudioOutputXT::~AudioOutputXT()
{
    if (m_audioPort) {
        xine_close_audio_driver(m_xine, m_audioPort);
        m_audioPort = 0;
        debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port destroyed";
    }
}

qreal AudioOutput::volume() const
{
    return m_volume;
}

int AudioOutput::outputDevice() const
{
    return m_device.index();
}

void AudioOutput::setVolume(qreal newVolume)
{
    m_volume = newVolume;

    int xinevolume = static_cast<int>(m_volume * 100);
    if (xinevolume > 200) {
        xinevolume = 200;
    } else if (xinevolume < 0) {
        xinevolume = 0;
    }

    upstreamEvent(new UpdateVolumeEvent(xinevolume));
    emit volumeChanged(m_volume);
}

xine_audio_port_t *AudioOutputXT::audioPort() const
{
    return m_audioPort;
}

static QByteArray audioDriverFor(const QByteArray &driver)
{
    if (driver == "alsa" || driver == "oss" || driver == "pulseaudio" || driver == "esd" ||
            driver == "arts" || driver == "jack") {
        return driver;
    }
    return QByteArray();
}

static bool lookupConfigEntry(xine_t *xine, const char *key, xine_cfg_entry_t *entry, const char *driver)
{
    if(!xine_config_lookup_entry(xine, key, entry)) {
        // the config key is not registered yet - it is registered when the output
        // plugin is opened. So we open the plugin and close it again, then we can set the
        // setting. :(
        xine_audio_port_t *port = xine_open_audio_driver(xine, driver, 0);
        if (port) {
            xine_close_audio_driver(xine, port);
            // port == 0 does not have to be fatal, since it might be only the default device
            // that cannot be opened
        }
        // now the config key should be registered
        if(!xine_config_lookup_entry(xine, key, entry)) {
            qWarning() << "cannot configure the device on Xine's" << driver << "output plugin";
            return false;
        }
    }
    return true;
}

xine_audio_port_t *AudioOutput::createPort(const AudioOutputDevice &deviceDesc)
{
    K_XT(AudioOutput);
    xine_audio_port_t *port = 0;

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        // Here we trust that the PA plugin is setup correctly and we just want to use it.
        const QByteArray &outputPlugin = "pulseaudio";
        debug() << Q_FUNC_INFO << "PA Active: use output plugin:" << outputPlugin;
        port = xine_open_audio_driver(xt->m_xine, outputPlugin.constData(), 0);
        debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port created";
        return port;
    }

    if (!deviceDesc.isValid()) {
        // use null output for invalid devices
        port = xine_open_audio_driver(xt->m_xine, "none", 0);
        debug() << Q_FUNC_INFO << "----------------------------------------------- null audio_port created";
        return port;
    }

    typedef QPair<QByteArray, QString> PhononDeviceAccess;
    QList<PhononDeviceAccess> deviceAccessList = deviceAccessListFor(deviceDesc);
    if (deviceAccessList.isEmpty()) {
        const QByteArray &outputPlugin = Backend::audioDriverFor(deviceDesc.index());
        if (outputPlugin == "alsa") {
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default"));
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default:CARD=0"));
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default:CARD=1"));
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default:CARD=2"));
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default:CARD=3"));
            deviceAccessList << PhononDeviceAccess("alsa", QLatin1String("default:CARD=4"));
        } else if (outputPlugin == "oss") {
            deviceAccessList << PhononDeviceAccess("oss", QLatin1String("/dev/dsp"));
            deviceAccessList << PhononDeviceAccess("oss", QLatin1String("/dev/dsp1"));
            deviceAccessList << PhononDeviceAccess("oss", QLatin1String("/dev/dsp2"));
            deviceAccessList << PhononDeviceAccess("oss", QLatin1String("/dev/dsp3"));
            deviceAccessList << PhononDeviceAccess("oss", QLatin1String("/dev/dsp4"));
        } else {
            debug() << Q_FUNC_INFO << "use output plugin:" << outputPlugin;
            port = xine_open_audio_driver(xt->m_xine, outputPlugin.constData(), 0);
            debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port created";
            return port;
        }
    }
    const QList<PhononDeviceAccess> &_deviceAccessList = deviceAccessList;
    foreach (const PhononDeviceAccess &access, _deviceAccessList) {
        const QByteArray &outputPlugin = audioDriverFor(access.first);
        if (outputPlugin.isEmpty()) {
            continue;
        }
        const QString &handle = access.second;
        if (outputPlugin == "alsa") {
            xine_cfg_entry_t deviceConfig;
            if (!lookupConfigEntry(xt->m_xine, "audio.device.alsa_default_device",
                        &deviceConfig, "alsa")) {
                continue;
            }
            Q_ASSERT(deviceConfig.type == XINE_CONFIG_TYPE_STRING);
            QByteArray deviceStr = handle.toUtf8();
            deviceConfig.str_value = deviceStr.data();
            xine_config_update_entry(xt->m_xine, &deviceConfig);

            const int err = xine_config_lookup_entry(xt->m_xine, "audio.device.alsa_front_device",
                    &deviceConfig);
            Q_ASSERT(err); Q_UNUSED(err);
            Q_ASSERT(deviceConfig.type == XINE_CONFIG_TYPE_STRING);
            deviceConfig.str_value = deviceStr.data();
            xine_config_update_entry(xt->m_xine, &deviceConfig);

            port = xine_open_audio_driver(xt->m_xine, "alsa", 0);
            if (port) {
                debug() << Q_FUNC_INFO << "use ALSA device: " << handle;
                debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port created";
                return port;
            }
        } else if (outputPlugin == "pulseaudio") {
            xine_cfg_entry_t deviceConfig;
            if (!lookupConfigEntry(xt->m_xine, "audio.pulseaudio_device", &deviceConfig,
                    "pulseaudio")) {
                continue;
            }
            Q_ASSERT(deviceConfig.type == XINE_CONFIG_TYPE_STRING);
            QByteArray deviceStr = handle.toUtf8();
            deviceStr.replace('\n', ':');
            deviceConfig.str_value = deviceStr.data();
            xine_config_update_entry(xt->m_xine, &deviceConfig);

            port = xine_open_audio_driver(xt->m_xine, "pulseaudio", 0);
            if (port) {
                debug() << Q_FUNC_INFO << "use PulseAudio: " << handle;
                debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port created";
                return port;
            }
        } else if (outputPlugin == "oss") {
            xine_cfg_entry_t deviceConfig;
            if (!lookupConfigEntry(xt->m_xine, "audio.device.oss_device_name", &deviceConfig,
                        "oss")) {
                continue;
            }
            Q_ASSERT(deviceConfig.type == XINE_CONFIG_TYPE_ENUM);
            deviceConfig.num_value = 0;
            xine_config_update_entry(xt->m_xine, &deviceConfig);
            if(!xine_config_lookup_entry(xt->m_xine, "audio.device.oss_device_number",
                        &deviceConfig)) {
                qWarning() << "cannot set the OSS device on Xine's OSS output plugin";
                return 0;
            }
            Q_ASSERT(deviceConfig.type == XINE_CONFIG_TYPE_NUM);
            const QByteArray &deviceStr = handle.toUtf8();
            char lastChar = deviceStr[deviceStr.length() - 1];
            int deviceNumber = -1;
            if (lastChar >= '0' || lastChar <= '9') {
                deviceNumber = lastChar - '0';
                char lastChar = deviceStr[deviceStr.length() - 2];
                if (lastChar >= '0' || lastChar <= '9') {
                    deviceNumber += 10 * (lastChar - '0');
                }
            }
            deviceConfig.num_value = deviceNumber;
            xine_config_update_entry(xt->m_xine, &deviceConfig);

            port = xine_open_audio_driver(xt->m_xine, "oss", 0);
            if (port) {
                debug() << Q_FUNC_INFO << "use OSS device: " << handle;
                debug() << Q_FUNC_INFO << "----------------------------------------------- audio_port created";
                return port;
            }
        }
    }
    return port;
}

bool AudioOutput::setOutputDevice(int newDevice)
{
    return setOutputDevice(AudioOutputDevice::fromIndex(newDevice));
}

bool AudioOutput::setOutputDevice(const AudioOutputDevice &newDevice)
{
    K_XT(AudioOutput);
    if (!xt->m_xine) {
        // remember the choice until we have a xine_t
        m_device = newDevice;
        return true;
    }

    xine_audio_port_t *port = createPort(newDevice);
    if (!port) {
        debug() << Q_FUNC_INFO << "new audio port is invalid";
        return false;
    }

    KeepReference<> *keep = new KeepReference<>;
    keep->addObject(xt);
    keep->ready();

    AudioOutputXT *newXt = new AudioOutputXT;
    newXt->m_audioPort = port;
    newXt->m_xine = xt->m_xine;
    m_threadSafeObject = newXt;

    m_device = newDevice;
    SourceNode *src = source();
    if (src) {
        QList<WireCall> wireCall;
        QList<WireCall> unwireCall;
        wireCall << WireCall(src, this);
        unwireCall << WireCall(src, QExplicitlySharedDataPointer<SinkNodeXT>(xt));
        QCoreApplication::postEvent(XineThread::instance(), new RewireEvent(wireCall, unwireCall));
        graphChanged();
    }

    AudioDataOutputXT *dataOutput = dynamic_cast<AudioDataOutputXT*>(m_source->threadSafeObject().data());
    if (dataOutput)
        dataOutput->intercept(xt->m_audioPort);

    return true;
}

void AudioOutput::xineEngineChanged()
{
    K_XT(AudioOutput);
    if (xt->m_xine) {
        xine_audio_port_t *port = createPort(m_device);
        if (!port) {
            debug() << Q_FUNC_INFO << "stored audio port is invalid";
            QMetaObject::invokeMethod(this, "audioDeviceFailed", Qt::QueuedConnection);
            return;
        }

        // our XT object is in a wirecall, better not delete it

        Q_ASSERT(xt->m_audioPort == 0);
        xt->m_audioPort = port;


        AudioDataOutputXT *dataOutput = dynamic_cast<AudioDataOutputXT*>(m_source->threadSafeObject().data());
        if (dataOutput)
            dataOutput->intercept(xt->m_audioPort);
    }
}

void AudioOutput::aboutToChangeXineEngine()
{
    K_XT(AudioOutput);
    if (xt->m_audioPort) {
        AudioOutputXT *xt2 = new AudioOutputXT;
        xt2->m_xine = xt->m_xine;
        xt2->m_audioPort = xt->m_audioPort;
        xt->m_audioPort = 0;
        KeepReference<> *keep = new KeepReference<>;
        keep->addObject(xt2);
        keep->ready();
    }
}

void AudioOutput::downstreamEvent(Event *e)
{
    Q_ASSERT(e);
    QCoreApplication::sendEvent(this, e);
    SinkNode::downstreamEvent(e);
}

void AudioOutputXT::rewireTo(SourceNodeXT *source)
{
    if (!source->audioOutputPort()) {
        return;
    }
    source->assert();
    xine_post_wire_audio_port(source->audioOutputPort(), m_audioPort);
    source->assert();
    SinkNodeXT::assert();
}

bool AudioOutput::event(QEvent *ev)
{
    switch (ev->type()) {
    case Event::AudioDeviceFailed:
        {
            ev->accept();
            // we don't know for sure which AudioPort failed. We also can't know from the
            // information libxine makes available. So we have to just try the old device again
            if (setOutputDevice(m_device)) {
                return true;
            }
            // we really need a different output device
            QMetaObject::invokeMethod(this, "audioDeviceFailed", Qt::QueuedConnection);
        }
        return true;
    default:
        return AbstractAudioOutput::event(ev);
    }
}

void AudioOutput::graphChanged()
{
    debug() << Q_FUNC_INFO;
    // we got connected to a new XineStream, it needs to know our m_volume
    int xinevolume = static_cast<int>(m_volume * 100);
    if (xinevolume > 200) {
        xinevolume = 200;
    } else if (xinevolume < 0) {
        xinevolume = 0;
    }
    upstreamEvent(new UpdateVolumeEvent(xinevolume));
}

}} //namespace Phonon::Xine

#include "audiooutput.moc"
// vim: sw=4 ts=4
