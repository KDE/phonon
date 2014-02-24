/*  This file is part of the KDE project
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>

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

#include "videoplayer.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "videowidget.h"
#include "path.h"
#include <QBoxLayout>
#include <QtCore/QEvent>

#ifndef QT_NO_PHONON_VIDEOPLAYER

namespace Phonon
{

class VideoPlayerPrivate
{
    public:
        VideoPlayerPrivate()
            : player(0)
            , aoutput(0)
            , voutput(0)
            , category(Phonon::NoCategory)
            , initialized(false) {}

        void ensureCreated() const;

        mutable MediaObject *player;
        mutable AudioOutput *aoutput;
        mutable VideoWidget *voutput;

        mutable MediaSource src;
        mutable Phonon::Category category;
        mutable bool initialized;
        VideoPlayer *q_ptr;
};

void VideoPlayerPrivate::ensureCreated() const
{
    if (!initialized) {
        initialized = true;
        QVBoxLayout *layout = new QVBoxLayout(q_ptr);
        layout->setMargin(0);

        aoutput = new AudioOutput(category, q_ptr);
        voutput = new VideoWidget(q_ptr);
        layout->addWidget(voutput);

        player = new MediaObject(q_ptr);
        Phonon::createPath(player, aoutput);
        Phonon::createPath(player, voutput);

        q_ptr->connect(player, SIGNAL(finished()), SIGNAL(finished()));
    }
}

VideoPlayer::VideoPlayer(Phonon::Category category, QWidget *parent)
    : QWidget(parent)
    , d(new VideoPlayerPrivate)
{
    d->q_ptr = this;
    d->category = category;
}

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , d(new VideoPlayerPrivate)
{
    d->q_ptr = this;
    d->category = Phonon::VideoCategory;
}

VideoPlayer::~VideoPlayer()
{
    delete d;
}

MediaObject *VideoPlayer::mediaObject() const
{
    d->ensureCreated();
    return d->player;
}

AudioOutput *VideoPlayer::audioOutput() const
{
    d->ensureCreated();
    return d->aoutput;
}

VideoWidget *VideoPlayer::videoWidget() const
{
    d->ensureCreated();
    return d->voutput;
}

void VideoPlayer::load(const MediaSource &source)
{
    d->ensureCreated();
    d->player->setCurrentSource(source);
}

void VideoPlayer::play(const MediaSource &source)
{
    d->ensureCreated();
    if (source == d->player->currentSource()) {
        if (!isPlaying())
            d->player->play();
        return;
    }
    // new URL
    d->player->setCurrentSource(source);
        
    if (ErrorState == d->player->state())
        return;

    d->player->play();
}

void VideoPlayer::play()
{
    d->ensureCreated();
    d->player->play();
}

void VideoPlayer::pause()
{
    d->ensureCreated();
    d->player->pause();
}

void VideoPlayer::stop()
{
    d->ensureCreated();
    d->player->stop();
}

qint64 VideoPlayer::totalTime() const
{
    d->ensureCreated();
    return d->player->totalTime();
}

qint64 VideoPlayer::currentTime() const
{
    d->ensureCreated();
    return d->player->currentTime();
}

void VideoPlayer::seek(qint64 ms)
{
    d->ensureCreated();
    d->player->seek(ms);
}

float VideoPlayer::volume() const
{
    d->ensureCreated();
    return d->aoutput->volume();
}

void VideoPlayer::setVolume(float v)
{
    d->ensureCreated();
    d->aoutput->setVolume(v);
}

bool VideoPlayer::isPlaying() const
{
    d->ensureCreated();
    return (d->player->state() == PlayingState);
}

bool VideoPlayer::isPaused() const
{
    d->ensureCreated();
    return (d->player->state() == PausedState);
}

bool VideoPlayer::event(QEvent *e) {
    if (e->type() == QEvent::Show)
        d->ensureCreated();
    return QWidget::event(e);
}

} // namespaces

#endif //QT_NO_PHONON_VIDEOPLAYER

#include "moc_videoplayer.cpp"

// vim: sw=4 ts=4
