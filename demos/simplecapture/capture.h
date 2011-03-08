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

#include <QtGui/QWidget>
#include <phonon/Global>
#include <phonon/Path>

class QPushButton;
class QRadioButton;

namespace Phonon {
    class AudioOutput;
    class MediaNode;
    class MediaObject;
    class VideoWidget;

    namespace Experimental {
        class AvCapture;
    }
}

/**
 * @brief Simple audio-video capture widget
 *
 * Can capture video using a media object, or audio-video using an
 * AvCapture object. Does not save the video, currently.
 *
 * The code is larger because both methods for capture are shown. Using
 * a single media object, you can't capture audio and video at the same time.
 *
 * AvCapture is essentialy two media objects, one for audio and one for video.
 *
 * In your application you only need to choose which method suits best and
 * ignore the other one. With media object it is similar to playing normal
 * media, and with AvCapture it is a little more intuitive.
 *
 * @note Phonon capture is still not stabilised, don't be surprised if audio
 * capture fails to work. The functionality also depends greatly on the backend.
 *
 * @see Phonon::MediaObject
 * @see Phonon::Experimental::AvCapture
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
    void mediaStateChanged(Phonon::State newState, Phonon::State oldState);

    /**
     * @brief Plays or pauses the media, depending on current state
     */
    void playPause();

    /**
     * @brief Stops the capture
     */
    void stop();

    /**
     * @brief Switch to MediaObject capture method
     *
     * Stops the capture, switches to the media object method and remakes
     * the media node connections
     *
     * @see setupCaptureSource
     */
    void enableMOCapture(bool enable);

    /**
     * @brief Switch to AvCapture capture method
     *
     * Stops the capture, switches to the avcapture method and remakes
     * the media node connections
     *
     * @see setupCaptureSource
     */
    void enableAvCapture(bool enable);

private:
    /**
     * Destroys any existing media paths, creates new ones depeding on the
     * capture method. Sets up the appropriate capture devices, obtained
     * from Phonon. Finally, it connects the state changed signal from
     * Phonon to our slot.
     *
     * @see Phonon::Path
     * @see Phonon::MediaObject
     * @see Phonon::Experimental::AvCapture
     */
    void setupCaptureSource();

private:
    Phonon::AudioOutput *m_audioOutput;
    Phonon::VideoWidget *m_videoWidget;
    Phonon::MediaNode *m_captureNode;               // Determines which capture method is used
    Phonon::MediaObject *m_media;                   // Media object for capture
    Phonon::Experimental::AvCapture *m_avcapture;   // AvCapture for capture
    Phonon::Path m_audioPath;                       // Path for audio
    Phonon::Path m_videoPath;                       // Path for video
    QPushButton *m_playButton;
    QPushButton *m_stopButton;
    QRadioButton *m_moButton;
    QRadioButton *m_avcapButton;
};

#endif // CAPTURE_H
