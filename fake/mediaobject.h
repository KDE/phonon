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

#ifndef PHONON_FAKE_MEDIAOBJECT_H
#define PHONON_FAKE_MEDIAOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <phonon/mediaobjectinterface.h>
#include <phonon/mediasource.h>

namespace Phonon {
namespace Fake {

class Pipeline : public QTimer
{
    Q_OBJECT
public:
    Pipeline() { reset(); }
    ~Pipeline() {}

    qint64 time() const { return m_time; }

    void start()
    {
        connect(this, SIGNAL(timeout()), SLOT(onTimeout()));
        QTimer::start();
    }

    void pause() { disconnect(this); }

    void stop()
    {
        pause();
        QTimer::stop();
        reset();
    }

    void reset() { m_time = 0; }

    void seek(qint64 msec) { m_time = msec; }

signals:
    void tick(qint64 time);

private slots:
    void onTimeout()
    {
        m_time += interval();
    }

private:
    qint64 m_time;
};

class MediaObject : public QObject, public MediaObjectInterface
{
    Q_OBJECT
public:
    MediaObject(QObject *parent = 0);
    ~MediaObject();

    // --------------------------- Implementation --------------------------- //
    virtual MediaSource source() const;
    virtual void setSource(const MediaSource &);
    virtual void setNextSource(const MediaSource &source);

    virtual void play();
    virtual void pause();
    virtual void stop();

    virtual bool isSeekable() const;
    virtual void seek(qint64 milliseconds);

    virtual qint32 tickInterval() const;
    virtual void setTickInterval(qint32 interval);

    virtual bool hasVideo() const;

    virtual qint64 currentTime() const;

    virtual Phonon::State state() const;

    virtual QString errorString() const;

    virtual Phonon::ErrorType errorType() const;

    virtual qint64 totalTime() const;
//    virtual qint64 remainingTime() const;

    virtual qint32 prefinishMark() const;
    virtual void setPrefinishMark(qint32);

    virtual qint32 transitionTime() const;
    virtual void setTransitionTime(qint32);
    // ---------------------------------------------------------------------- //

signals:
    // --------------------------- Implementation --------------------------- //
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void tick(qint64 time);
    // ---------------------------------------------------------------------- //

private:
    void changeState(Phonon::State newState);

    MediaSource m_source;
    MediaSource m_nextSource;
    State m_state;

    // Fake Helpers
    Pipeline m_pipeline;
};

} // namespace Fake
} // namespace Phonon

#endif // PHONON_FAKE_MEDIAOBJECT_H
