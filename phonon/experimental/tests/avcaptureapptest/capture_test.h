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

#ifndef PHONON_TEST_H
#define PHONON_TEST_H

#include <Qt/QtCore>
#include <Qt/QtGui>
#include <backendcapabilities.h>
#include <objectdescription.h>
#include <audiooutput.h>
#include <videoplayer.h>
#include <videowidget.h>
#include <volumeslider.h>
#include <mediaobject.h>
#include <globalconfig.h>
#include <objectdescriptionmodel.h>
#include <experimental/avcapture.h>

class MediaPlayer : public QWidget
{
    Q_OBJECT

    public:
        MediaPlayer(QWidget *parent);
        ~MediaPlayer();

    public slots:
        void setVideoDeviceIndex(int index);
        void setAudioDeviceIndex(int index);

    private:
        void updateDeviceList();

    private:
        Phonon::Experimental::AVCapture *m_avcapture;
        Phonon::AudioOutput *m_aoutput;
        Phonon::VideoWidget *m_vwidget;
        Phonon::VolumeSlider *m_volumeSlider;
        Phonon::VideoCaptureDeviceModel *m_videoDeviceModel;
        Phonon::AudioCaptureDeviceModel *m_audioDeviceModel;

        QPushButton *m_playButton;
        QPushButton *m_stopButton;
        QComboBox *m_videoDeviceCombo;
        QComboBox *m_audioDeviceCombo;
};


#endif



