/*
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2013-2014 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_PLAYER_H
#define PHONON_PLAYER_H

#include "frontend.h"
#include "source.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Phonon {

class AbstractOutput;
class PlayerPrivate;

// FIXME: ADD convenience statics for play() with limited control
// FIXME: should effects be set on the player rather than on the output? vlc doesnt really support per-output

class PHONON_EXPORT Player : public QObject, public Frontend
{
    Q_OBJECT
    // FIXME: maybe there should be a conveience function setTime to align names?
    // FIXME: the go to class QSlider uses int, so one runs into signal mismatches
    //        possibly we will want a compat interface.
    Q_PROPERTY(qint64 time READ time WRITE seek NOTIFY timeChanged)
    Q_PROPERTY(qint64 totalTime READ totalTime NOTIFY totalTimeChanged)
    // FIXME: needs executive decision on whether we want remaining time at all.
    Q_PROPERTY(qint64 remainingTime READ remainingTime NOTIFY remainingTimeChanged)
    Q_PROPERTY(Source source READ source WRITE setSource NOTIFY sourceChanged)
    // FIXME: notify missing
    // FIXME: needs executive decision whether we really want this in the core class
    //        seeing as this is a convenience function for QML it might be better
    //        suitable for the actual qml module.
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY stoppedChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
public:
    Player(QObject *parent = 0);
    ~Player();

    // FIXME: replace tick interval by static tick 100 whenever something is connected to timeChanged
    qint32 tickInterval() const;

    State state() const;

    QStringList metaData(MetaData key) const;
    QMultiMap<MetaData, QString> metaData() const;

    qint64 time() const;
    qint64 totalTime() const;
    qint64 remainingTime() const;

    Source source() const;
    void setSource(const Source &source);

    QUrl url() const;
    void setUrl(const QUrl &url);

    bool isSeekable() const;
    bool isPlaying() const;
    bool isStopped() const;
    bool isPaused() const;

// NOTE: error magic TBD
    //    QString errorString() const;
    //    ErrorType errorType() const;

public Q_SLOTS:
    // Outputs are unordered and all considered to be equal.
    // FIXME: QML wants a qobject interface or it won't call it...
    bool addOutput(QObject *output);

    void setTickInterval(qint32 newTickInterval);

    void seek(qint64 time);

    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);

    void metaDataChanged();

    void seekableChanged(bool isSeekable);

    // Caps at 100 == finished.
    void bufferStatus(int percentFilled);

    void timeChanged(qint64 time);
    void totalTimeChanged(qint64 newTotalTime);
    // FIXME: noop right now.
    void remainingTimeChanged(qint64 remainingTime);

    // We may not want this here at all and simply assume that changes via setSource are instant.
    // Additionally source tracking would happen through the playlist then?
    // For QML a sourceChanged would still be handy though.
    void sourceChanged(const Source &newSource);

    void playingChanged(bool playing);
    void pausedChanged(bool paused);
    void stoppedChanged(bool stopped);

private:
    P_DECLARE_PRIVATE(Player)
    Q_PRIVATE_SLOT(k_func(), void _p_metaDataChanged(const QMultiMap<MetaData, QString> &))
    Q_PRIVATE_SLOT(k_func(), void _p_stateChanged(Phonon::State, Phonon::State))
};

} // namespace Phonon

#endif // PHONON_PLAYER_H
