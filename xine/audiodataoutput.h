/*  This file is part of the KDE project
    Copyright (C) 2004 Max Howell <max.howell@methylblue.com>
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2009 Martin Sandsmark <sandsmark@samfundet.no>

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
#ifndef Phonon_XINE_AUDIODATAOUTPUT_H
#define Phonon_XINE_AUDIODATAOUTPUT_H


#include "audiooutput.h"
#include "sourcenode.h"
#include "sinknode.h"
#include "events.h"


#include <phonon/audiodataoutput.h>
#include <phonon/audiodataoutputinterface.h>

#include <QLinkedList>

extern "C" {
    #define this xine_this //HACK; Xine uses “this” as a name for certain variables
    #define XINE_ENGINE_INTERNAL // We need the port_ticket
    #include <xine/audio_out.h>
    #include <xine/post.h>
    #undef XINE_ENGINE_INTERNAL
    #undef this
}


namespace Phonon
{
namespace Xine
{
class AudioDataOutputXT;
class AudioDataOutput;

// Our Xine plugin struct
typedef struct
{
    post_plugin_t post;

    AudioDataOutputXT *audioDataOutput;
} scope_plugin_t;


// Struct for storing an audio frame with timestamp
typedef struct
{
    QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > map;
    qint64 timestamp;
} Frame;

class AudioDataOutputXT : public SinkNodeXT, public SourceNodeXT
{
    public:
        AudioDataOutputXT(AudioDataOutput *output);
        ~AudioDataOutputXT();

        xine_post_out_t *audioOutputPort() const;
        xine_audio_port_t *audioPort() const { return m_audioPort; }

        void intercept(xine_audio_port_t*, bool isNull = false);

        //callback functions
        static int  openPort(xine_audio_port_t*, xine_stream_t*, uint32_t, uint32_t, int);
        static void closePort(xine_audio_port_t *, xine_stream_t *);
        static void putBufferCallback(xine_audio_port_s*, audio_buffer_s* buf, xine_stream_s* stream);
        static void dispose(post_plugin_t*);


    private:
        void rewireTo(SourceNodeXT *);

        AudioDataOutput    *m_frontend;
        xine_audio_port_t *m_audioPort;
        scope_plugin_t       *m_plugin;
        int                 m_channels;
        xine_post_out_t  *m_postOutput;
        int64_t            m_firstVpts;

}; // class AudioDataOutputXT

/**
 * \author Martin Sandsmark <sandsmark@samfundet.no>
 */
class AudioDataOutput : public QObject,
                        public Phonon::Xine::SinkNode,
                        public Phonon::Xine::SourceNode,
                        public Phonon::AudioDataOutputInterface

{
    Q_OBJECT

    Phonon::AudioDataOutput* m_frontend;
    Q_INTERFACES(Phonon::AudioDataOutputInterface Phonon::Xine::SinkNode Phonon::Xine::SourceNode)

    public:
        AudioDataOutput(QObject *parent);
        ~AudioDataOutput();

        //Getters
        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio; }
        MediaStreamTypes outputMediaStreamTypes() const { return Phonon::Xine::Audio; }
        Phonon::AudioDataOutput *frontendObject() const { return m_frontend; }
        int channels() const { return m_channels; }
        int dataSize() const { return m_dataSize; }
        int sampleRate() const { return m_sampleRate; }

        void upstreamEvent(Event*);

        friend class AudioDataOutputXT;

    public slots:
        //Setters
        void setFrontendObject(Phonon::AudioDataOutput *frontend) { m_frontend = frontend; }
        void setChannels(int channels) { m_channels = channels; m_pendingFrames.clear(); }
        void setDataSize(int ds) { m_dataSize = ds; m_pendingFrames.clear(); }

    signals:
        void dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &data);
        void dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<float> > &data);
        void endOfMedia(int remainingSamples);

    private:
        void packetReady(const int samples, const qint16 *buffer, const qint64 vpts);

        int                           m_channels;
        int                           m_dataSize;
        int                         m_sampleRate;
        QList<Frame>       m_pendingFrames;
        bool                        m_keepInSync;
        MediaObject               *m_mediaObject;

}; //class AudioDataOutput

}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_AUDIODATAOUTPUT_H
