/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_MEDIAOBJECT_H
#define Phonon_MEDIAOBJECT_H

#include "medianode.h"
#include "mediasource.h"
#include "phonon_export.h"
#include "phonondefs.h"
#include "phononnamespace.h"


namespace Phonon
{
    class MediaObjectPrivate;

    class PHONON_EXPORT MediaObject : public QObject, public MediaNode
    {
        friend class FrontendInterfacePrivate;
        Q_OBJECT
        P_DECLARE_PRIVATE(MediaObject)
        PHONON_OBJECT(MediaObject)


        Q_PROPERTY(qint32 tickInterval READ tickInterval WRITE setTickInterval)
        public:
            /**
             * Destroys the MediaObject.
             */
            ~MediaObject();

            /**
             * Get the current state.
             *
             * @return The state of the object.
             *
             * @see State
             * \see stateChanged
             */
            State state() const;

            /**
             * Check whether the media data includes a video stream.
             *
             * \warning This information cannot be known immediately. It is best
             * to also listen to the hasVideoChanged signal.
             *
             * \code
             *   connect(media, SIGNAL(hasVideoChanged(bool)), hasVideoChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->hasVideo(); // returns false;
             * }
             *
             * void hasVideoChanged(bool b)
             * {
             *   // b == true
             *   media->hasVideo(); // returns true;
             * }
             * \endcode
             *
             * \return \c true if the media contains video data. \c false
             * otherwise.
             *
             * \see hasVideoChanged
             */
            bool hasVideo() const;

            /**
             * Check whether the current media may be seeked.
             *
             * \warning This information cannot be known immediately. It is best
             * to also listen to the seekableChanged signal.
             *
             * \code
             *   connect(media, SIGNAL(seekableChanged(bool)), seekableChanged(bool));
             *   media->setCurrentSource("somevideo.avi");
             *   media->isSeekable(); // returns false;
             * }
             *
             * void seekableChanged(bool b)
             * {
             *   // b == true
             *   media->isSeekable(); // returns true;
             * }
             * \endcode
             *
             * \return \c true when the current media may be seeked. \c false
             * otherwise.
             *
             * \see seekableChanged()
             */
            bool isSeekable() const;

            /**
             * \brief The time interval in milliseconds between two ticks.
             *
             * The %tick interval is the time that elapses between the emission
             * of two tick signals.
             *
             * \returns the tick interval in milliseconds
             */
            qint32 tickInterval() const;

            QStringList metaData(const QString &key) const;

            /**
             * Returns the strings associated with the given \p key.
             *
             * Same as above except that the keys are defined in the
             * Phonon::MetaData enum.
             */
            QStringList metaData(Phonon::MetaData key) const;

            /**
             * Returns all meta data.
             */
            QMultiMap<QString, QString> metaData() const;

            /**
             * Returns a human-readable description of the last error that occurred.
             */
            QString errorString() const;

            /**
             * Tells your program what to do about the error.
             *
             * \see Phonon::ErrorType
             */
            ErrorType errorType() const;

            /**
             * Returns the current media source.
             *
             * \see setCurrentSource
             */
            MediaSource currentSource() const;

            /**
             * Set the media source the MediaObject should use.
             *
             * \param source The MediaSource object to the media data. You can
             * just as well use a QUrl or QString (for a local file) here.
             * Setting an empty (invalid) source, will stop and remove the
             * current source.
             *
             * \code
             * QUrl url("http://www.example.com/music.ogg");
             * media->setCurrentSource(url);
             * \endcode
             *
             * \see currentSource
             */
            void setCurrentSource(const MediaSource &source);

            /**
             * Get the current time (in milliseconds) of the file currently being played.
             *
             * \return The current time in milliseconds.
             *
             * \see tick
             */
            qint64 currentTime() const;

            /**
             * Get the total time (in milliseconds) of the file currently being played.
             *
             * \return The total time in milliseconds.
             *
             * \note The total time may change throughout playback as more accurate
             *       calculations become available, so it is recommended to connect
             *       and use the totalTimeChanged signal whenever possible unless
             *       best precision is not of importance.
             *
             * \warning The total time is undefined until the MediaObject entered
             *          the PlayingState. A valid total time is always indicated by
             *          emission of the totalTimeChanged signal.
             * \see totalTimeChanged
             */
            qint64 totalTime() const;

            /**
             * Get the remaining time (in milliseconds) of the file currently being played.
             *
             * \return The remaining time in milliseconds.
             */
            qint64 remainingTime() const;

        public Q_SLOTS:

            /**
             * Sets the tick interval in milliseconds.
             *
             * \param newTickInterval the new tick interval in milliseconds.
             *
             * \see tickInterval
             */
            void setTickInterval(qint32 newTickInterval);

            /**
             * Requests playback of the media data to start. Playback only
             * starts when stateChanged() signals that it goes into PlayingState,
             * though.
             *
             * \par Possible states right after this call:
             * \li BufferingState
             * \li PlayingState
             * \li ErrorState
             */
            void play();

            /**
             * Requests playback to pause. If it was paused before nothing changes.
             *
             * \par Possible states right after this call:
             * \li PlayingState
             * \li PausedState
             * \li ErrorState
             */
            void pause();

            /**
             * Requests playback to stop. If it was stopped before nothing changes.
             *
             * \par Possible states right after this call:
             * \li the state it was in before (e.g. PlayingState)
             * \li StoppedState
             * \li ErrorState
             */
            void stop();

            /**
             * Requests a seek to the time indicated.
             *
             * You can only seek if state() == PlayingState, BufferingState or PausedState.
             *
             * The call is asynchronous, so currentTime can still be the old
             * value right after this method was called. If all you need is a
             * slider that shows the current position and allows the user to
             * seek use the class SeekSlider.
             *
             * @param time The time in milliseconds where to continue playing.
             *
             * \par Possible states right after this call:
             * \li BufferingState
             * \li PlayingState
             * \li ErrorState
             *
             * \see SeekSlider
             */
            void seek(qint64 time);

        Q_SIGNALS:
            /**
             * Emitted when the state of the MediaObject has changed.
             *
             * @param newstate The state the Player is in now.
             * @param oldstate The state the Player was in before.
             */
            void stateChanged(Phonon::State newstate, Phonon::State oldstate);

            /**
             * This signal gets emitted every tickInterval milliseconds.
             *
             * @param time The position of the media file in milliseconds.
             *
             * @see setTickInterval, tickInterval
             */
            void tick(qint64 time);

            /**
             * This signal is emitted whenever the audio/video data that is
             * being played is associated with new meta data. E.g. for radio
             * streams this happens when the next song is played.
             *
             * You can get the new meta data with the metaData methods.
             */
            void metaDataChanged();

            /**
             * Emitted whenever the return value of isSeekable() changes.
             *
             * Normally you'll check isSeekable() first and then let this signal
             * tell you whether seeking is possible now or not. That way you
             * don't have to poll isSeekable().
             *
             * \param isSeekable \p true  if the stream is seekable (i.e. calling
             *                            seek() works)
             *                   \p false if the stream is not seekable (i.e.
             *                            all calls to seek() will be ignored)
             */
            void seekableChanged(bool isSeekable);

            /**
             * Emitted whenever the return value of hasVideo() changes.
             *
             * Normally you'll check hasVideo() first and then let this signal
             * tell you whether video is available now or not. That way you
             * don't have to poll hasVideo().
             *
             * \param hasVideo \p true  The stream contains video and adding a
             *                          VideoWidget will show a video.
             *                 \p false There is no video data in the stream and
             *                          adding a VideoWidget will show an empty (black)
             *                          VideoWidget.
             */
#ifndef QT_NO_PHONON_VIDEO
            void hasVideoChanged(bool hasVideo);
#endif //QT_NO_PHONON_VIDEO

            /**
             * Tells about the status of the buffer.
             *
             * You can use this signal to show a progress bar to the user when
             * in BufferingState:
             *
             * \code
             * progressBar->setRange(0, 100); // this is the default
             * connect(media, SIGNAL(bufferStatus(int)), progressBar, SLOT(setValue(int)));
             * \endcode
             *
             * \param percentFilled A number between 0 and 100 telling you how
             *                      much the buffer is filled.
             */ // other names: bufferingProgress
            void bufferStatus(int percentFilled);

            /**
             * Emitted when the object has finished playback.
             * It is not emitted if you call stop(), pause() or
             * load(), but only on end-of-queue or a critical error.
             *
             * \warning This signal is not emitted when the current source has
             * finished and there's another source in the queue. It is only
             * emitted when the queue is empty.
             *
             * \see currentSourceChanged
             * \see aboutToFinish
             * \see prefinishMarkReached
             */
            void finished();

            /**
             * Emitted when the MediaObject makes a transition to the next
             * MediaSource in the queue().
             *
             * In other words, it is emitted when an individual MediaSource is
             * finished.
             *
             * \param newSource The source that starts to play at the time the
             * signal is emitted.
             */
            void currentSourceChanged(const Phonon::MediaSource &newSource);

            /**
             * This signal is emitted as soon as the total time of the media file is
             * known or has changed. For most non-local media data the total
             * time of the media can only be known after some time. Initially the
             * totalTime function can not return useful information. You have
             * to wait for this signal to know the real total time.
             *
             * This signal may appear at any given point after a MediaSource was set.
             * Namely in the LoadingState, BufferingState, PlayingState or PausedState.
             *
             * \note When changing the currentSource there is no signal emission until
             *       a reasonable value for the new source has been calculated.
             *
             * \param newTotalTime The length of the media file in milliseconds.
             *
             * \see totalTime
             */
            void totalTimeChanged(qint64 newTotalTime);

        protected:
            //MediaObject(Phonon::MediaObjectPrivate &dd, QObject *parent);

        private:
            Q_PRIVATE_SLOT(k_func(), void _k_resumePlay())
            Q_PRIVATE_SLOT(k_func(), void _k_resumePause())
            Q_PRIVATE_SLOT(k_func(), void _k_metaDataChanged(const QMultiMap<QString, QString> &))
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_currentSourceChanged(const MediaSource &))
            Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
    };
} //namespace Phonon


// vim: sw=4 ts=4 tw=80
#endif // Phonon_MEDIAOBJECT_H
