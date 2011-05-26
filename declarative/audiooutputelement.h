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

#ifndef AUDIOOUTPUTELEMENT_H
#define AUDIOOUTPUTELEMENT_H

#include "abstractmediaelement.h"
#include <QtDeclarative/QDeclarativeParserStatus>

#include <phonon/phononnamespace.h>

namespace Phonon {

class AudioOutput;

namespace Declarative {

class AudioOutputElement : public AbstractMediaElement, public QDeclarativeParserStatus
{
    Q_OBJECT
#warning writing those is fishy as it is not exactly clear what happens when you \
    set play to false, does it pause? or stop? or error? or explode?!!!
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY stoppedChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    AudioOutputElement(QObject *parent = 0);
    ~AudioOutputElement();

    void classBegin() {};
    void componentComplete() {};

    bool isPlaying() const;
    bool isStopped() const;

signals:
    void playingChanged();
    void stoppedChanged();

public slots:
    void play();
    void stop();

private slots:
    void handleFinished();
    void handleStateChange(Phonon::State newState, Phonon::State oldState);

private:
    virtual void init();
    void emitStateChanges(Phonon::State state);

    AudioOutput *m_audioOutput;
    State m_state;

    bool m_finished;
};

} // namespace Declarative
} // namespace Phonon

#endif // AUDIOOUTPUTELEMENT_H
