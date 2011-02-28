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
#include <QtGui/QRadioButton>
#include <QtGui/QMessageBox>

#include <phonon/AudioOutput>
#include <phonon/AvCapture>
#include <phonon/MediaObject>
#include <phonon/VideoWidget>
#include <phonon/VolumeSlider>

#include <phonon/backendcapabilities.h>
#include <phonon/globalconfig.h>

CaptureWidget::CaptureWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    m_media = new Phonon::MediaObject(this);
    m_avcapture = new Phonon::Experimental::AvCapture(this);
    m_captureNode = m_media;

    m_audioOutput = new Phonon::AudioOutput(this);
    m_videoWidget = new Phonon::VideoWidget(this);

    m_playButton = new QPushButton(this);
    m_playButton->setText(tr("Play"));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playPause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText(tr("Stop"));
    m_stopButton->setEnabled(false);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    m_moButton = new QRadioButton(this);
    m_moButton->setText(tr("Use MediaObject"));
    m_moButton->setAutoExclusive(true);
    m_moButton->setChecked(true);
    connect(m_moButton, SIGNAL(toggled(bool)), this, SLOT(enableMOCapture(bool)));

    m_avcapButton = new QRadioButton(this);
    m_avcapButton->setText(tr("Use Audio Video Capture (AvCapture)"));
    m_avcapButton->setAutoExclusive(true);
    connect(m_avcapButton, SIGNAL(toggled(bool)), this, SLOT(enableAvCapture(bool)));

    setLayout(new QVBoxLayout);

    m_videoWidget->setMinimumSize(QSize(400, 300));
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(m_videoWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_moButton);
    buttonsLayout->addWidget(m_avcapButton);
    layout()->addItem(buttonsLayout);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_stopButton);
    layout()->addItem(buttonsLayout);

    setupCaptureSource();
    playPause();
}

void CaptureWidget::enableMOCapture(bool enable)
{
    if (!enable)
        return;

    stop();
    m_captureNode = m_media;
    setupCaptureSource();
}

void CaptureWidget::enableAvCapture(bool enable)
{
    if (!enable)
        return;

    stop();
    m_captureNode = m_avcapture;
    setupCaptureSource();
}

void CaptureWidget::setupCaptureSource()
{
    if (m_audioPath.isValid()) {
        m_audioPath.disconnect();
    }
    if (m_videoPath.isValid()) {
        m_videoPath.disconnect();
    }

    if (m_captureNode == m_media) {
        m_audioPath = Phonon::createPath(m_media, m_audioOutput);
        m_videoPath = Phonon::createPath(m_media, m_videoWidget);
    }
    if (m_captureNode == m_avcapture) {
        m_audioPath = Phonon::createPath(m_avcapture, m_audioOutput);
        m_videoPath = Phonon::createPath(m_avcapture, m_videoWidget);
    }

    if (!m_audioPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support audio capturing.");
    }
    if (!m_videoPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support video capturing.");
    }

    if (m_captureNode == m_media) {
        Phonon::GlobalConfig pgc;
        m_media->setCurrentSource(Phonon::VideoCaptureDevice::fromIndex(pgc.videoCaptureDeviceFor(Phonon::NoCategory)));
    }
    if (m_captureNode == m_avcapture) {
        m_avcapture->setVideoCaptureDevice(Phonon::NoCategory);
    }

    if (m_captureNode == m_avcapture) {
        m_avcapture->setAudioCaptureDevice(Phonon::NoCategory);
    }

    if (m_captureNode == m_media) {
        disconnect(m_avcapture, SIGNAL(stateChanged(Phonon::State,Phonon::State)));
        connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State, Phonon::State)));
    }
    if (m_captureNode == m_avcapture) {
        disconnect(m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)));
        connect(m_avcapture, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State,Phonon::State)));
    }
}

void CaptureWidget::playPause()
{
    if (m_captureNode == m_media) {
        if (m_media->state() == Phonon::PlayingState) {
            m_media->pause();
        } else {
            m_media->play();
        }
    }

    if (m_captureNode == m_avcapture) {
        if (m_avcapture->state() == Phonon::PlayingState) {
            m_avcapture->pause();
        } else {
            m_avcapture->start();
        }
    }
}

void CaptureWidget::stop()
{
    if (m_captureNode == m_media) {
        m_media->stop();
    }

    if (m_captureNode == m_avcapture) {
        m_avcapture->stop();
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
