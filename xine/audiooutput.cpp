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
#include <kdebug.h>

#include <sys/ioctl.h>
#include <iostream>
#include <QSet>
#include "mediaobject.h"
#include "backend.h"
#include "events.h"
#include "wirecall.h"
#include "xineengine.h"
#include "xinethread.h"
#include "keepreference.h"

#include <xine/audio_out.h>

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
    //kDebug(610) ;
}

AudioOutputXT::~AudioOutputXT()
{
    if (m_audioPort) {
        xine_close_audio_driver(m_xine, m_audioPort);
        m_audioPort = 0;
        kDebug(610) << "----------------------------------------------- audio_port destroyed";
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

xine_audio_port_t *AudioOutput::createPort(const AudioOutputDevice &deviceDesc)
{
    K_XT(AudioOutput);
    xine_audio_port_t *port = 0;

    QVariant v = deviceDesc.property("driver");
    if (!v.isValid()) {
        const QByteArray &outputPlugin = Backend::audioDriverFor(deviceDesc.index());
        kDebug(610) << "use output plugin:" << outputPlugin;
        port = xine_open_audio_driver(xt->m_xine, outputPlugin.constData(), 0);
    } else {
        const QByteArray &outputPlugin = v.toByteArray();
        v = deviceDesc.property("deviceIds");
        const QStringList &deviceIds = v.toStringList();
        if (deviceIds.isEmpty()) {
            return 0;
        }
        //kDebug(610) << outputPlugin << alsaDevices;

        if (outputPlugin == "alsa") {
            foreach (const QString &device, deviceIds) {
                xine_cfg_entry_t alsaDeviceConfig;
                QByteArray deviceStr = device.toUtf8();
                if(!xine_config_lookup_entry(xt->m_xine, "audio.device.alsa_default_device",
                            &alsaDeviceConfig)) {
                    // the config key is not registered yet - it is registered when the alsa output
                    // plugin is opened. So we open the plugin and close it again, then we can set the
                    // setting. :(
                    port = xine_open_audio_driver(xt->m_xine, "alsa", 0);
                    if (port) {
                        xine_close_audio_driver(xt->m_xine, port);
                        // port == 0 does not have to be fatal, since it might be only the default device
                        // that cannot be opened
                    }
                    // now the config key should be registered
                    if(!xine_config_lookup_entry(xt->m_xine, "audio.device.alsa_default_device",
                                &alsaDeviceConfig)) {
                        kError(610) << "cannot set the ALSA device on Xine's ALSA output plugin";
                        return 0;
                    }
                }
                Q_ASSERT(alsaDeviceConfig.type == XINE_CONFIG_TYPE_STRING);
                alsaDeviceConfig.str_value = deviceStr.data();
                xine_config_update_entry(xt->m_xine, &alsaDeviceConfig);

                int err = xine_config_lookup_entry(xt->m_xine, "audio.device.alsa_front_device", &alsaDeviceConfig);
                Q_ASSERT(err);
                Q_ASSERT(alsaDeviceConfig.type == XINE_CONFIG_TYPE_STRING);
                alsaDeviceConfig.str_value = deviceStr.data();
                xine_config_update_entry(xt->m_xine, &alsaDeviceConfig);

                port = xine_open_audio_driver(xt->m_xine, "alsa", 0);
                if (port) {
                    kDebug(610) << "use ALSA device: " << device;
                    break;
                }
            }
        } else if (outputPlugin == "oss") {
            kDebug(610) << "use OSS output";
            port = xine_open_audio_driver(xt->m_xine, "oss", 0);
        }
    }
    kDebug(610) << "----------------------------------------------- audio_port created";
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
        // remeber the choice until we have a xine_t
        m_device = newDevice;
        return true;
    }

    xine_audio_port_t *port = createPort(newDevice);
    if (!port) {
        kDebug(610) << "new audio port is invalid";
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
    return true;
}

void AudioOutput::xineEngineChanged()
{
    K_XT(AudioOutput);
    if (xt->m_xine) {
        xine_audio_port_t *port = createPort(m_device);
        if (!port) {
            kDebug(610) << "stored audio port is invalid";
            QMetaObject::invokeMethod(this, "audioDeviceFailed", Qt::QueuedConnection);
            return;
        }

        // our XT object is in a wirecall, better not delete it

        Q_ASSERT(xt->m_audioPort == 0);
        xt->m_audioPort = port;
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
            // we don't know for sure which AudioPort failed. but the one without any
            // capabilities must be the guilty one
            K_XT(AudioOutput);
            if (!xt->m_audioPort || xt->m_audioPort->get_capabilities(xt->m_audioPort) == AO_CAP_NOCAP) {
                QMetaObject::invokeMethod(this, "audioDeviceFailed", Qt::QueuedConnection);
            }
        }
        return true;
    default:
        return AbstractAudioOutput::event(ev);
    }
}

void AudioOutput::graphChanged()
{
    kDebug(610);
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
