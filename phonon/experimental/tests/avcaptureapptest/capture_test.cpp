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

    m_avcapture = new Phonon::Experimental::AvCapture();

    Phonon::createPath(m_avcapture, m_aoutput);
    Phonon::createPath(m_avcapture, m_vwidget);

    QHBoxLayout *deviceNameLayout = new QHBoxLayout(this);

    m_videoDeviceCombo = new QComboBox(this);
    m_videoDeviceCombo->setEditable(false);
    m_audioDeviceCombo = new QComboBox(this);
    m_audioDeviceCombo->setEditable(false);
    deviceNameLayout->addWidget(m_videoDeviceCombo);
    deviceNameLayout->addWidget(m_audioDeviceCombo);
    connect(m_videoDeviceCombo, SIGNAL(activated(int)), this, SLOT(setVideoDeviceIndex(int)));
    connect(m_audioDeviceCombo, SIGNAL(activated(int)), this, SLOT(setAudioDeviceIndex(int)));
    updateDeviceList();

    layout->addItem(deviceNameLayout);

    m_playButton = new QPushButton(this);
    m_playButton->setText("Start");
    connect(m_playButton, SIGNAL(clicked()), m_avcapture, SLOT(start()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText("Stop");
    connect(m_stopButton, SIGNAL(clicked()), m_avcapture, SLOT(stop()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_stopButton);
    layout->addItem(buttonsLayout);

    m_volumeSlider = new Phonon::VolumeSlider(this);
    layout->addWidget(m_volumeSlider);
    m_volumeSlider->setAudioOutput(m_aoutput);

    setLayout(layout);
}

MediaPlayer::~MediaPlayer()
{
    delete m_aoutput;
    delete m_avcapture;
}

void MediaPlayer::setVideoDeviceIndex(int index)
{
#ifndef QT_NO_PHONON_VIDEOCAPTURE
    QModelIndex mi = m_videoDeviceModel->index(index, 0, QModelIndex());
    Q_ASSERT(mi.isValid());

    Phonon::VideoCaptureDevice vc = m_videoDeviceModel->modelData(mi);
    Q_ASSERT(vc.isValid());

    m_avcapture->setVideoCaptureDevice(vc);
#endif
}

void MediaPlayer::setAudioDeviceIndex(int index)
{
#ifndef QT_NO_PHONON_AUDIOCAPTURE
    QModelIndex mi = m_audioDeviceModel->index(index, 0, QModelIndex());
    Q_ASSERT(mi.isValid());

    Phonon::AudioCaptureDevice ac = m_audioDeviceModel->modelData(mi);
    Q_ASSERT(ac.isValid());

    m_avcapture->setAudioCaptureDevice(ac);
#endif // QT_NO_PHONON_AUDIOCAPTURE
}

void MediaPlayer::updateDeviceList()
{
#ifndef QT_NO_PHONON_VIDEOCAPTURE
    QList<Phonon::VideoCaptureDevice> vl = Phonon::BackendCapabilities::availableVideoCaptureDevices();

    if (!m_videoDeviceModel)
        m_videoDeviceModel = new Phonon::VideoCaptureDeviceModel(vl, 0);
    m_videoDeviceCombo->setModel(m_videoDeviceModel);

    Q_ASSERT(m_videoDeviceModel->rowCount() >= 0);

    if (m_videoDeviceModel->rowCount() == 0)
        QMessageBox::critical(this, "Error", "No video capture devices found.");
#else
    QMessageBox::critical(this, "Error", "Video capture is disabled.");
#endif

#ifndef QT_NO_PHONON_AUDIOCAPTURE
    QList<Phonon::AudioCaptureDevice> al = Phonon::BackendCapabilities::availableAudioCaptureDevices();

    if (!m_audioDeviceModel)
        m_audioDeviceModel = new Phonon::AudioCaptureDeviceModel(al, 0);
    m_audioDeviceCombo->setModel(m_audioDeviceModel);

    Q_ASSERT(m_audioDeviceModel->rowCount() >= 0);

    if (m_audioDeviceModel->rowCount() == 0)
        QMessageBox::critical(this, "Error", "No audio capture devices found.");
#else
    QMessageBox::critical(this, "Error", "Audio capture is disabled.");
#endif
}

