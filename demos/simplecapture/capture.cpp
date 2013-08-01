/*  This file is part of the KDE project
    Copyright (C) 2011 Casian Andrei <skeletk13@gmail.com>

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

#include "capture.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/VideoWidget>

CaptureWidget::CaptureWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    // Create the objects used for capture
    m_media = new Phonon::MediaObject(this);

    // Create the audio and video outputs (sinks)
    m_audioOutput = new Phonon::AudioOutput(this);
    m_videoWidget = new Phonon::VideoWidget(this);

    /*
     * Set up the buttons and layouts and widgets
     */
    m_playButton = new QPushButton(this);
    m_playButton->setText(tr("Play"));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playPause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText(tr("Stop"));
    m_stopButton->setEnabled(false);
    connect(m_stopButton, SIGNAL(clicked()), m_media, SLOT(stop()));

    setLayout(new QVBoxLayout);

    // Configure the video widget a bit
    m_videoWidget->setMinimumSize(QSize(400, 300));
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(m_videoWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_stopButton);
    layout()->addItem(buttonsLayout);

    /*
     * Create the paths from the capture object to the outputs
     * If the paths are invalid, then probably the backend doesn't support capture
     */
    Phonon::Path audioPath = Phonon::createPath(m_media, m_audioOutput);
    Phonon::Path videoPath = Phonon::createPath(m_media, m_videoWidget);

    if (!audioPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support audio capturing.");
    }
    if (!videoPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support video capturing.");
    }

    /*
     * Set up the devices used for capture
     * Phonon can easily get you the devices appropriate for a specific category.
     */
    Phonon::MediaSource source(Phonon::Capture::VideoType, Phonon::NoCaptureCategory);
    m_media->setCurrentSource(source);

    // Connect the stateChanged signal from the media object used for capture
    connect(m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(mediaStateChanged(Phonon::State)));

    // Start capturing
    playPause();
}

void CaptureWidget::playPause()
{
    if (m_media->state() == Phonon::PlayingState) {
        m_media->pause();
    } else {
        m_media->play();
    }
}

void CaptureWidget::mediaStateChanged(Phonon::State newState)
{
    switch(newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_playButton->setText(tr("Play"));
        m_stopButton->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playButton->setText(tr("Pause"));
        m_stopButton->setEnabled(true);
        break;
    case Phonon::BufferingState:
        break;
    case Phonon::PausedState:
        m_playButton->setText(tr("Play"));
        break;
    case Phonon::ErrorState:
        break;
    }
}
