/*  This file is part of the KDE project
    Copyright (C) 2010 Trever Fischer <tdfischer@fedoraproject.org>

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
#include "player.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/Mrl>
#include <phonon/SeekSlider>
#include <phonon/VideoWidget>

Player::Player(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    m_media = new Phonon::MediaObject(this);

    //Some platforms (i.e. Linux) provide a mechanism for a user to view a system-wide
    //history of content interactions. This is opt-in, and done via setting the
    //PlaybackTracking property to true.
    m_media->setProperty("PlaybackTracking", true);

    Phonon::AudioOutput* audioOut = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::VideoWidget* videoOut = new Phonon::VideoWidget(this);

    videoOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //By default, there is no minimum size on a video widget. While the default
    //size is controlled by Qt's layouting system it makes sense to provide a
    //minimum size, so that the widget does not disappear, when the user resizes
    //the window.
    videoOut->setMinimumSize(64, 64);

    //After a MediaSource is loaded, this signal will be emitted to let us know
    //if a video stream was found.
    connect(m_media, SIGNAL(hasVideoChanged(bool)), videoOut, SLOT(setVisible(bool)));

    //Link the media object to our audio and video outputs.
    Phonon::createPath(m_media, audioOut);
    Phonon::createPath(m_media, videoOut);

    //This widget will contain the stop/pause buttons
    QWidget *buttonBar = new QWidget(this);

    m_playPause = new QPushButton(tr("Play"), buttonBar);
    m_stop = new QPushButton(tr("Stop"), buttonBar);

    Phonon::SeekSlider *seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setMediaObject(m_media);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(videoOut);
    layout->addWidget(seekSlider);
    layout->addWidget(buttonBar);
    setLayout(layout);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->addWidget(m_stop);
    buttonLayout->addWidget(m_playPause);
    buttonBar->setLayout(buttonLayout);

    m_stop->setEnabled(false);

    connect(m_stop, SIGNAL(clicked(bool)), m_media, SLOT(stop()));
    connect(m_playPause, SIGNAL(clicked(bool)), this, SLOT(playPause()));

    //The mediaStateChanged slot will update the GUI elements to reflect what
    //the user can do next
    connect(m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State,Phonon::State)));
}

void Player::playPause()
{
    if (m_media->state() == Phonon::PlayingState) {
        m_media->pause();
    } else {
        if (m_media->currentSource().type() == Phonon::MediaSource::Empty)
            load();
        m_media->play();
    }
}

void Player::load(const Phonon::Mrl &mrl)
{
    if (mrl.scheme().isEmpty())
        m_media->setCurrentSource(Phonon::Mrl::fromLocalFile(mrl.toString()));
    else
        m_media->setCurrentSource(mrl);
    m_media->play();
}

void Player::load()
{
    QString url = QFileDialog::getOpenFileName(this);
    if (url.isEmpty())
        return;
    load(Phonon::Mrl::fromLocalFile(url));
}

void Player::mediaStateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch(newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_playPause->setText(tr("Play"));
        m_stop->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playPause->setText(tr("Pause"));
        m_stop->setEnabled(true);
        break;
    case Phonon::BufferingState:
        break;
    case Phonon::PausedState:
        m_playPause->setText(tr("Play"));
        break;
    case Phonon::ErrorState:
        QMessageBox::critical(this, tr("Error"), tr("Error while playing media: ") + m_media->errorString());
        break;
    }
}
