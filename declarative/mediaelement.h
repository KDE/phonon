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

#include <phonon/phononnamespace.h>

#include "abstractinitable.h"

namespace Phonon {

class MediaObject;

namespace Declarative {

class MediaElement : public QDeclarativeItem, AbstractInitAble
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
#warning writing those is fishy as it is not exactly clear what happens when you \
    set play to false, does it pause? or stop? or error? or explode?!!!
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY stoppedChanged)
    Q_PROPERTY(qreal totalTime READ totalTime NOTIFY totalTimeChanged)
    Q_PROPERTY(qreal time READ time WRITE seek NOTIFY timeChanged)
public:
    MediaElement(QDeclarativeItem *parent = 0);
    virtual ~MediaElement();

    void classBegin();
    void componentComplete();

    QUrl source() const;
    void setSource(const QUrl &url);

    virtual void init(MediaObject *mediaObject = 0);

    MediaObject *mediaObject() const { return m_mediaObject; }

    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    qreal totalTime() const;
    qreal time() const;
    void seek(qreal time);

signals:
    void sourceChanged();

    void stateChanged();
    void playingChanged();
    void pausedChanged();
    void stoppedChanged();

    void totalTimeChanged();
    void timeChanged();

public slots:
    void play();
    void pause();
    void stop();

private slots:
    void handleFinished();
    void handleStateChange(Phonon::State newState, Phonon::State oldState);

protected:
    State m_state;

    bool m_finished;

private:
    void emitStateChanges(Phonon::State state);

    QUrl m_source;
};

} // namespace Declarative
} // namespace Phonon

#endif // ABSTRACTMEDIAELEMENT_H
