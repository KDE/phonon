/*  This file is part of the KDE project
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

#ifndef PHONON_XINE_XINESTREAM_H
#define PHONON_XINE_XINESTREAM_H

#include "sourcenode.h"

#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QMutex>
#include <QtCore/QMultiMap>
#include <QtCore/QWaitCondition>
#include <QtCore/QTimer>

#include <Phonon/Global>
#include <Phonon/ObjectDescription>

#include <xine.h>

#include <sys/time.h>
#include <time.h>
#include "xineengine.h"
#include "myshareddatapointer.h"

class KUrl;

namespace Phonon
{
namespace Xine
{
class VideoWidget;
class SeekCommandEvent;
class MediaObject;
class ByteStream;

/**
 * \brief xine_stream_t wrapper that runs in its own thread.
 *
 * The xine_stream_t object is created as late as possible so that it doesn't have to be recreated
 * when an audio- or video_port is added.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class XineStream : public QObject, public SourceNodeXT
{
    Q_OBJECT
    public:
        static void xineEventListener(void *, const xine_event_t *);

        XineStream();
        ~XineStream();

        Phonon::State state() const { return m_state; }

        int totalTime() const;
        int remainingTime() const;
        int currentTime() const;
        bool hasVideo() const;
        bool isSeekable() const;

        /*
        void addAudioPostList(const AudioPostList &);
        void removeAudioPostList(const AudioPostList &);
        */

        void setTickInterval(qint32 interval);
        void setPrefinishMark(qint32 time);

        //void needRewire(AudioPostList *postList);
        void useGaplessPlayback(bool);
        void useGapOf(int gap);
        void gaplessSwitchTo(const KUrl &url);
        void gaplessSwitchTo(const QByteArray &mrl);
        void closeBlocking();
        void aboutToDeleteVideoWidget();
        /*VideoWidget *videoWidget() const
        {
            if (m_newVideoPort) {
                return m_newVideoPort;
            }
            return m_videoPort;
        }*/

        void setError(Phonon::ErrorType, const QString &);
        QString errorString() const;
        Phonon::ErrorType errorType() const;

        int availableChapters() const { return m_availableChapters; }
        int availableAngles()   const { return m_availableAngles;   }
        int availableTitles()   const { return m_availableTitles;   }
        int currentChapter()    const { return m_currentChapter;    }
        int currentAngle()      const { return m_currentAngle;      }
        int currentTitle()      const { return m_currentTitle;      }

        QList<AudioChannelDescription> availableAudioChannels() const;
        QList<SubtitleDescription> availableSubtitles() const;

        AudioChannelDescription currentAudioChannel() const;
        SubtitleDescription currentSubtitle() const;

        void setCurrentAudioChannel(const AudioChannelDescription& streamDesc);
        void setCurrentSubtitle(const SubtitleDescription& streamDesc);

        int subtitlesSize() const;
        int audioChannelsSize() const;

        enum StateForNewMrl {
            // no use: Loading, Error, Buffering
            StoppedState = Phonon::StoppedState,
            PlayingState = Phonon::PlayingState,
            PausedState = Phonon::PausedState,
            KeepState = 0xff
        };

        xine_post_out_t *audioOutputPort() const;
        xine_post_out_t *videoOutputPort() const;

        xine_audio_port_t *nullAudioPort() const;
        xine_video_port_t *nullVideoPort() const;
        XineEngine xine() const;

        void setMediaObject(MediaObject *m) { m_mediaObject = m; }
        void handleDownstreamEvent(Event *e);

    public slots:
        void setUrl(const KUrl &url);
        void setMrl(const QByteArray &mrl, StateForNewMrl = StoppedState);
        void play();
        void pause();
        void stop();
        void seek(qint64 time);

        /**
         * all signals emitted from the xine thread
         */
    Q_SIGNALS:
        void finished();
        void stateChanged(Phonon::State newstate, Phonon::State oldstate);
        void metaDataChanged(const QMultiMap<QString, QString> &);
        void length(qint64);
        void seekDone();
        void needNextUrl();
        void tick(qint64);
        void prefinishMarkReached(qint32);
        void seekableChanged(bool);
        void hasVideoChanged(bool);
        void bufferStatus(int);

        void availableSubtitlesChanged();
        void availableAudioChannelsChanged();
        void availableChaptersChanged(int);
        void chapterChanged(int);
        void availableAnglesChanged(int);
        void angleChanged(int);
        void availableTitlesChanged(int);
        void titleChanged(int);
        void downstreamEvent(Event *e);

        void hackSetProperty(const char *name, const QVariant &val);

    protected:
        bool event(QEvent *ev);
        void timerEvent(QTimerEvent *event);

    private slots:
        void getStartTime();
        void emitAboutToFinish();
        void emitTick();

    private slots:
        void playbackFinished();

    private:
        void getStreamInfo();
        bool xineOpen(Phonon::State);
        void updateMetaData();
        bool createStream();
        void changeState(Phonon::State newstate);
        void emitAboutToFinishIn(int timeToAboutToFinishSignal);
        bool updateTime();
        void error(Phonon::ErrorType, const QString &);
        void internalPause();
        void internalPlay();
        void setMrlInternal(const QByteArray &newMrl);
        template<class S>
        S streamDescription(int index, uint hash, ObjectDescriptionType type, int(*get_xine_stream_text)(xine_stream_t *stream, int channel, char *lang)) const;
        uint streamHash() const;

        xine_stream_t *m_stream;
        xine_event_queue_t *m_event_queue;
        xine_post_t *m_deinterlacer;
        mutable XineEngine m_xine;
        mutable xine_audio_port_t *m_nullAudioPort;
        mutable xine_video_port_t *m_nullVideoPort;

        Phonon::State m_state;

        QMutex m_portMutex;
        mutable QReadWriteLock m_errorLock;
        mutable QMutex m_mutex;
        mutable QMutex m_streamInfoMutex;
        mutable QMutex m_updateTimeMutex;
        mutable QWaitCondition m_waitingForStreamInfo;
        QWaitCondition m_waitingForClose;
        QWaitCondition m_waitingForRewire;
        QMultiMap<QString, QString> m_metaDataMap;
        QByteArray m_mrl;
        MySharedDataPointer<ByteStream> m_byteStream;
        QTimer *m_prefinishMarkTimer;
        struct timeval m_lastTimeUpdate;

        QString m_errorString;
        Phonon::ErrorType m_errorType;

        MediaObject *m_mediaObject;
        SeekCommandEvent *m_lastSeekCommand;
        qint32 m_prefinishMark;
        int m_volume;
        int m_startTime;
        int m_totalTime;
        int m_currentTime;
        int m_waitForPlayingTimerId;
        int m_availableSubtitles;
        int m_availableAudioChannels;
        int m_availableTitles;
        int m_availableChapters;
        int m_availableAngles;
        int m_currentAngle;
        int m_currentTitle;
        int m_currentChapter;
        int m_transitionGap;
        bool m_streamInfoReady : 1;
        bool m_hasVideo : 1;
        bool m_isSeekable : 1;
        bool m_useGaplessPlayback : 1;
        bool m_prefinishMarkReachedNotEmitted : 1;
        bool m_ticking : 1;
        bool m_closing : 1;
        bool m_eventLoopReady : 1;
        QTimer m_tickTimer;
};

} // namespace Xine
} // namespace Phonon

#endif // PHONON_XINE_XINESTREAM_H
