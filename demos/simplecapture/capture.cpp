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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/VideoWidget>
#include <phonon/VolumeSlider>
#include <phonon/VideoCaptureDeviceModel>
#include <phonon/AudioCaptureDeviceModel>
#include <phonon/backendcapabilities.h>
#include <phonon/globalconfig.h>

CaptureWidget::CaptureWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(this);
    Phonon::VideoWidget *videoWidget = new Phonon::VideoWidget(this);
    Phonon::VideoCaptureDeviceModel *videoDeviceModel = new Phonon::VideoCaptureDeviceModel(this);
    Phonon::AudioCaptureDeviceModel *audioDeviceModel = new Phonon::AudioCaptureDeviceModel(this);

    setLayout(new QVBoxLayout);

    videoWidget->setMinimumSize(QSize(400, 300));
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(videoWidget);

    m_media = new Phonon::MediaObject(this);
    Phonon::createPath(m_media, audioOutput);
    Phonon::createPath(m_media, videoWidget);

    m_playButton = new QPushButton(this);
    m_playButton->setText(tr("Play"));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playPause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText(tr("Stop"));
    m_stopButton->setEnabled(false);
    connect(m_stopButton, SIGNAL(clicked()), m_media, SLOT(stop()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_stopButton);
    layout()->addItem(buttonsLayout);

    QList<Phonon::VideoCaptureDevice> lv = Phonon::BackendCapabilities::availableVideoCaptureDevices();
    if (!lv.isEmpty()) {
        Phonon::MediaSource source(lv.first());
        m_media->setCurrentSource(source);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("No video capture devices found."));
    }

    connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State, Phonon::State)));

    m_media->play();
}

void CaptureWidget::playPause()
{
    if (m_media->state() == Phonon::PlayingState) {
        m_media->pause();
    } else {
        m_media->play();
    }
}

void CaptureWidget::mediaStateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
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

#include "capture.moc"
