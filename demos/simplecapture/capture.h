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

#ifndef CAPTURE_H
#define CAPTURE_H

#include <QWidget>
#include <phonon/Global>

class QPushButton;

namespace Phonon {
    class AudioOutput;
    class MediaObject;
    class VideoWidget;
}

/**
 * @brief Simple audio-video capture widget
 *
 * Can capture video using a media object.
 *
 * Using just a single media object, you can't capture audio and video 
 * at the same time, if the desired device knows only audio or only video.
 *
 * @note Phonon capture is still not stabilised, don't be surprised if audio
 * capture fails to work. The functionality also depends on the backend.
 *
 * @see Phonon::MediaObject
 * @see Phonon::MediaSource
 */
class CaptureWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Capture widget constructor
     *
     * Sets up the GUI and Phonon objects.
     * Sets up the capture source and starts the capture.
     *
     * @see setupCaptureSource
     */
    CaptureWidget(QWidget *parent = NULL, Qt::WindowFlags f = 0);

private slots:
    /**
     * @brief Updates the GUI when the underlying MediaObject changes states
     *
     * Changes the caption of the play / pause button, depending on the state.
     * Enables or disables the stop button.
     */
    void mediaStateChanged(Phonon::State newState);

    /**
     * @brief Plays or pauses the media, depending on current state
     */
    void playPause();

private:
    Phonon::AudioOutput *m_audioOutput;
    Phonon::VideoWidget *m_videoWidget;
    Phonon::MediaObject *m_media;                   // Media object for capture
    QPushButton *m_playButton;
    QPushButton *m_stopButton;
};

#endif // CAPTURE_H
