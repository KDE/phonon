/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2008 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_XINE_AUDIOOUTPUT_H
#define Phonon_XINE_AUDIOOUTPUT_H

#include "abstractaudiooutput.h"
#include <QFile>

#include "xineengine.h"
#include <xine.h>
#include "xinestream.h"
#include <phonon/audiooutputinterface.h>
#include "connectnotificationinterface.h"

namespace Phonon
{
namespace Xine
{

class AudioOutputXT : public SinkNodeXT
{
    friend class AudioOutput;
    public:
        AudioOutputXT() : SinkNodeXT("AudioOutput"), m_audioPort(0) {}
        ~AudioOutputXT();
        void rewireTo(SourceNodeXT *);
        xine_audio_port_t *audioPort() const;

    private:
        xine_audio_port_t *m_audioPort;
};

class AudioOutput : public AbstractAudioOutput, public AudioOutputInterface, public ConnectNotificationInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::AudioOutputInterface Phonon::Xine::ConnectNotificationInterface)
    public:
        AudioOutput(QObject *parent);
        ~AudioOutput();

        // Attributes Getters:
        qreal volume() const;
        int outputDevice() const;

        // Attributes Setters:
        void setVolume(qreal newVolume);
        bool setOutputDevice(int newDevice);
        bool setOutputDevice(const AudioOutputDevice &newDevice);

        void downstreamEvent(Event *);

        virtual void graphChanged();

    protected:
        bool event(QEvent *);
        void xineEngineChanged();
        void aboutToChangeXineEngine();

    Q_SIGNALS:
        // for the Frontend
        void volumeChanged(qreal newVolume);
        void audioDeviceFailed();

    private:
        xine_audio_port_t *createPort(const AudioOutputDevice &device);

        qreal m_volume;
        AudioOutputDevice m_device;
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_AUDIOOUTPUT_H
