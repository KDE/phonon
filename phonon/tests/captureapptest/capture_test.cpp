/*  This file is part of the KDE project
 *  Copyright (C) 2010 Casian Andrei <skeletk13@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), Nokia Corporation
 *  (or its successors, if any) and the KDE Free Qt Foundation, which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "capture_test.h"

MediaPlayer::MediaPlayer(QWidget *parent)
: QWidget(parent)
{
    m_videoDeviceModel = NULL;
    m_audioDeviceModel = NULL;

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_vwidget = new Phonon::VideoWidget(this);
    m_vwidget->setMinimumSize(QSize(400, 300));
    m_vwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_vwidget);

    m_aoutput = new Phonon::AudioOutput();

    m_media = new Phonon::MediaObject();

    Phonon::createPath(m_media, m_aoutput);
    Phonon::createPath(m_media, m_vwidget);

    QHBoxLayout *deviceNameLayout = new QHBoxLayout();

    m_videoCaptureButton = new QRadioButton("Video", this);
    m_audioCaptureButton = new QRadioButton("Audio", this);

    connect(m_videoCaptureButton, SIGNAL(pressed()), this, SLOT(setVideoCapture()));
    connect(m_audioCaptureButton, SIGNAL(pressed()), this, SLOT(setAudioCapture()));

    m_deviceNameCombo = new QComboBox(this);
    m_deviceNameCombo->setEditable(false);
    connect(m_deviceNameCombo, SIGNAL(activated(int)), this, SLOT(setDeviceIndex(int)));
    updateDeviceList();

    deviceNameLayout->addWidget(m_videoCaptureButton);
    deviceNameLayout->addWidget(m_audioCaptureButton);
    deviceNameLayout->addWidget(m_deviceNameCombo);

    layout->addItem(deviceNameLayout);

    m_playButton = new QPushButton(this);
    m_playButton->setText("Play");
    connect(m_playButton, SIGNAL(clicked()), m_media, SLOT(play()));

    m_pauseButton = new QPushButton(this);
    m_pauseButton->setText("Pause");
    connect(m_pauseButton, SIGNAL(clicked()), m_media, SLOT(pause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText("Stop");
    connect(m_stopButton, SIGNAL(clicked()), m_media, SLOT(stop()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_pauseButton);
    buttonsLayout->addWidget(m_stopButton);
    layout->addItem(buttonsLayout);

    m_volumeSlider = new Phonon::VolumeSlider(this);
    layout->addWidget(m_volumeSlider);
    m_volumeSlider->setAudioOutput(m_aoutput);
}

MediaPlayer::~MediaPlayer()
{
    delete m_aoutput;
    delete m_media;
}

void MediaPlayer::setVideoCapture()
{
    m_deviceNameCombo->setModel(m_videoDeviceModel);
    m_vwidget->setEnabled(true);
    setDeviceIndex(0);
}

void MediaPlayer::setAudioCapture()
{
    m_deviceNameCombo->setModel(m_audioDeviceModel);
    m_vwidget->setEnabled(false);
    setDeviceIndex(0);
}

void MediaPlayer::setDeviceIndex(int index)
{
    Phonon::MediaSource mediaSource;
    QModelIndex mi;

#ifndef QT_NO_PHONON_VIDEOCAPTURE
    if (m_videoCaptureButton->isChecked()) {
        mi = m_videoDeviceModel->index(index, 0, QModelIndex());
        Q_ASSERT(mi.isValid());

        Phonon::VideoCaptureDevice vc = m_videoDeviceModel->modelData(mi);
        Q_ASSERT(vc.isValid());

        mediaSource = Phonon::MediaSource(vc);
        Q_ASSERT(!mediaSource.deviceAccessList().isEmpty());
        Q_ASSERT(!mediaSource.deviceAccessList().first().first.isEmpty());
        Q_ASSERT(!mediaSource.deviceAccessList().first().second.isEmpty());
    }
#endif // QT_NO_PHONON_VIDEOCAPTURE

#ifndef QT_NO_PHONON_AUDIOCAPTURE
    if (m_audioCaptureButton->isChecked()) {
        mi = m_audioDeviceModel->index(index, 0, QModelIndex());
        Q_ASSERT(mi.isValid());

        Phonon::AudioCaptureDevice ac = m_audioDeviceModel->modelData(mi);
        Q_ASSERT(ac.isValid());

        mediaSource = Phonon::MediaSource(ac);
        Q_ASSERT(!mediaSource.deviceAccessList().isEmpty());
        Q_ASSERT(!mediaSource.deviceAccessList().first().first.isEmpty());
        Q_ASSERT(!mediaSource.deviceAccessList().first().second.isEmpty());
    }
#endif // QT_NO_PHONON_AUDIOCAPTURE

    m_media->setCurrentSource(mediaSource);
    m_media->play();
}

void MediaPlayer::updateDeviceList()
{
    #ifndef QT_NO_PHONON_VIDEOCAPTURE
    QList<Phonon::VideoCaptureDevice> lv = Phonon::BackendCapabilities::availableVideoCaptureDevices();

    if (!m_videoDeviceModel)
        m_videoDeviceModel = new Phonon::VideoCaptureDeviceModel(lv, 0);

    Q_ASSERT(m_videoDeviceModel->rowCount() >= 0);

    if (m_videoDeviceModel->rowCount() == 0)
        QMessageBox::warning(this, "Warning", "No video capture devices found.");
    #else
    QMessageBox::critical(this, "Error", "Video capture is disabled.");
    #endif // QT_NO_PHONON_VIDEOCAPTURE

    #ifndef QT_NO_PHONON_AUDIOCAPTURE
    QList<Phonon::AudioCaptureDevice> la = Phonon::BackendCapabilities::availableAudioCaptureDevices();

    if (!m_audioDeviceModel)
        m_audioDeviceModel = new Phonon::AudioCaptureDeviceModel(la, 0);

    Q_ASSERT(m_audioDeviceModel->rowCount() >= 0);

    if (m_videoDeviceModel->rowCount() == 0)
        QMessageBox::warning(this, "Warning", "No audio capture devices found.");
    #else
    QMessageBox::critical(this, "Error", "Audio capture is disabled.");
    #endif // QT_NO_PHONON_AUDIOCAPTURE
}

