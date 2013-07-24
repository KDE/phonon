/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#ifndef ABSTRACTMEDIAELEMENT_H
#define ABSTRACTMEDIAELEMENT_H

#include <QtCore/QUrl>
#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeParserStatus>

#include <phonon/phononnamespace.h>

#include "abstractinitable.h"

namespace Phonon {
namespace Declarative {

class MetaData;

/**
 * This is the Qt Quick Element encasing a Phonon::VideoGraphicsObject.
 * For general information regarding capabilities please see the documentation
 * of Phonon::VideoGraphicsObject.
 *
 * Like every Phonon Qt Quick class this class provides semi-lazy initialization
 * as provided described by the AbstractInitAble class.
 *
 * This element cannot be decorated by another output. If you still try to do
 * so the output will simply attach to the MediaObject this VideoOutputElement
 * was attached to.
 *
 * \see Phonon::VideoGraphicsObject
 * \author Harald Sitter <sitter@kde.org>
 */
class MediaElement : public QDeclarativeItem, AbstractInitAble
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY stoppedChanged)

    Q_PROPERTY(bool video READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qint32 tickInterval READ tickInterval WRITE setTickInterval)
    Q_PROPERTY(qreal totalTime READ totalTime NOTIFY totalTimeChanged)
    Q_PROPERTY(qreal time READ time WRITE seek NOTIFY timeChanged)

    Q_PROPERTY(Phonon::Declarative::MetaData *metaData READ metaData CONSTANT)
public:
    MediaElement(QDeclarativeItem *parent = 0);
    virtual ~MediaElement();

    /// \reimp
    void classBegin();

    /// \reimp
    void componentComplete();

    /// \reimp
    virtual void init(MediaObject *mediaObject = 0);

    MediaObject *mediaObject() const { return m_mediaObject; }

    /// \see Phonon::MediaObject::currentSource
    QUrl source() const;

    /// \see Phonon::MediaObject::setCurrentSource
    void setSource(const QUrl &url);

    /// \returns \c true when current state is playing, \c false otherwise
    bool isPlaying() const;

    /// \returns \c true when current state is paused, \c false otherwise
    bool isPaused() const;

    /// \returns \c true when current state is stopped, \c false otherwise
    bool isStopped() const;

    /// \see Phonon::MediaObject::hasVideo
    bool hasVideo() const;

    /// \see Phonon::MediaObject::isSeekable
    bool isSeekable() const;

    /// \see Phonon::MediaObject::tickInterval
    qint32 tickInterval() const;

    /// \see Phonon::MediaObject::setTickInterval
    void setTickInterval(qint32 interval);

    /// \see Phonon::MediaObject::totalTime
    qreal totalTime() const;

    /// \see Phonon::MediaObject::currentTime
    qreal time() const;

    /// \see Phonon::MediaObject::seek
    void seek(qreal time);

    /**
     * The MetaData held within the element. The MetaData is static and thus this
     * call is relatively cheap.
     * The MetaData stays valid for as long as the MediaElement exists.
     *
     * \returns the MetaData container of this MediaElement
     */
    MetaData *metaData();

signals:
    /// emitted when the (current) source changed
    void sourceChanged();

    /// emitted when the state changed
    void stateChanged();

    /// emitted when the playing property changed
    void playingChanged();

    /// emitted when the paused property changed
    void pausedChanged();

    /// emitted when the stopped property changed
    void stoppedChanged();

    /// emitted when a video was detected
    void hasVideoChanged();

    /// emitted when the seekability changed
    void seekableChanged();

    /// emitted when the total time (length) changed
    void totalTimeChanged();

    /// emitted when the current playback time (elapsed time) changed
    void timeChanged();

public slots:
    /// Starts playing.
    void play();

    /// Pauses media (if it was playing -> no effect otherwise).
    void pause();

    /// Stops playing.
    void stop();

private slots:
    /// Called when MediaObject is aboutToFinish
    void handleFinished();

    /// Translates a state change to explicit singnals on the properties that changed
    void handleStateChange(Phonon::State newState, Phonon::State oldState);

protected:
    /// The current state of the media.
    State m_state;

    /// Whether or not the media has finished playing.
    bool m_finished;

private:
    /// Translates a state change to an explicit signal for playing, paused etc.
    void emitStateChanges(Phonon::State state);

    /// The current source
    QUrl m_source;

    /// The MetaData container of this element (lazy-init, so null by default)
    MetaData *m_metaData;
};

} // namespace Declarative
} // namespace Phonon

#endif // ABSTRACTMEDIAELEMENT_H
