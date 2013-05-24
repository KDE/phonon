/*
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_PLAYERINTERFACE_H
#define PHONON_PLAYERINTERFACE_H

#include "mediaobject.h"
#include <QtCore/QObject>

namespace Phonon {

/** \class MediaObjectInterface mediaobjectinterface.h phonon/MediaObjectInterface
 * \short Backend interface for media sources.
 *
 * The backend implementation has to provide two signals, that are not defined
 * in this interface:
 * <ul>
 * <li>\anchor phonon_MediaObjectInterface_stateChanged
 * <b>void stateChanged(\ref Phonon::State newstate, \ref Phonon::State oldstate)</b>
 *
 * Emitted when the state of the MediaObject has changed.
 * In case you're not interested in the old state you can also
 * connect to a slot that only has one State argument.
 *
 * \param newstate The state the Player is in now.
 * \param oldstate The state the Player was in before.
 * </li>
 * <li>\anchor phonon_MediaObjectInterface_tick
 * <b>void tick(qint64 time)</b>
 *
 * This signal gets emitted every tickInterval milliseconds.
 *
 * \param time The position of the media file in milliseconds.
 *
 * \see setTickInterval()
 * \see tickInterval()
 * </li>
 * </ul>
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \see MediaObject
 */
class PlayerInterface
{
public:
    virtual ~PlayerInterface() {}

    /**
     * Requests the playback to start.
     *
     * This method is only called if the state transition to \ref PlayingState is possible.
     *
     * The backend should react immediately
     * by either going into \ref PlayingState or \ref BufferingState if the
     * former is not possible.
     */
    virtual void play() = 0;

    /**
     * Requests the playback to pause.
     *
     * This method is only called if the state transition to \ref PausedState is possible.
     *
     * The backend should react as fast as possible. Go to \ref PausedState
     * as soon as playback is paused.
     */
    virtual void pause() = 0;

    /**
     * Requests the playback to be stopped.
     *
     * This method is only called if the state transition to \ref StoppedState is possible.
     *
     * The backend should react as fast as possible. Go to \ref StoppedState
     * as soon as playback is stopped.
     *
     * A subsequent call to play() will start playback at the beginning of
     * the media.
     */
    virtual void stop() = 0;

    /**
     * Requests the playback to be seeked to the given time.
     *
     * The backend does not have to finish seeking while in this function
     * (i.e. the backend does not need to block the thread until the seek is
     * finished; even worse it might lead to deadlocks when using a
     * ByteStream which gets its data from the thread this function would
     * block).
     *
     * As soon as the seek is done the currentTime() function and
     * the tick() signal will report it.
     *
     * \param milliseconds The time where playback should seek to in
     * milliseconds.
     */
    virtual void seek(qint64 milliseconds) = 0;

    /**
     * Return the time interval in milliseconds between two ticks.
     *
     * \returns Returns the tick interval that it was set to (might not
     * be the same as you asked for).
     */
    virtual qint32 tickInterval() const = 0;
    /**
     * Change the interval the tick signal is emitted. If you set \p
     * interval to 0 the signal gets disabled.
     *
     * \param interval tick interval in milliseconds
     *
     * \returns Returns the tick interval that it was set to (might not
     *          be the same as you asked for).
     */
    virtual void setTickInterval(qint32 interval) = 0;

    /**
     * If the current media may be seeked returns true.
     *
     * \returns whether the current media may be seeked.
     */
    virtual bool isSeekable() const = 0;
    /**
     * Get the current time (in milliseconds) of the file currently being played.
     */
    virtual qint64 currentTime() const = 0;
    /**
     * Get the current state.
     */
    virtual Phonon::State state() const = 0;

    /**
     * A translated string describing the error.
     */
    virtual QString errorString() const = 0;

    /**
     * Tells your program what to do about the error.
     *
     * \see Phonon::ErrorType
     */
    virtual Phonon::ErrorType errorType() const = 0;

    /**
     * Returns the total time of the media in milliseconds.
     *
     * If the total time is not know return -1. Do not block until it is
     * known, instead emit the totalTimeChanged signal as soon as the total
     * time is known or changes.
     */
    virtual qint64 totalTime() const = 0;

    /**
     * Returns the current source.
     */
    virtual Source source() const = 0;

    /**
     * Sets the current source. When this function is called the MediaObject is
     * expected to stop all current activity and start loading the new
     * source (i.e. go into LoadingState).
     *
     * It is expected that the
     * backend now starts preloading the media data, filling the audio
     * and video buffers and making all media meta data available. It
     * will also trigger the totalTimeChanged signal.
     *
     * If the backend does not know how to handle the source it needs to
     * change state to Phonon::ErrorState. Don't bother about handling KIO
     * URLs. It is enough to handle AbstractMediaStream sources correctly.
     *
     * \warning Keep the MediaSource object around as long as the backend
     * uses the AbstractMediaStream returned by the MediaSource. In case
     * that no other reference to the MediaSource exists and it is set to
     * MediaSource::autoDelete, the AbstractMediaStream is deleted when the
     * last MediaSource ref is deleted.
     */
    virtual void setSource(const Source &) = 0;

    virtual qint64 remainingTime() const { return totalTime() - currentTime(); }

    virtual void addAudioOutput(QObject *audioOutput) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::PlayerInterface, "org.kde.phonon.PlayerInterface/5.0")

#endif // PHONON_PLAYERINTERFACE_H
