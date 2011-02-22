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

class QPushButton;
namespace Phonon {
    class MediaObject;
}

/**
 * @brief Simple audio-video capture widget
 */
class CaptureWidget : public QWidget
{
    Q_OBJECT

public:
    CaptureWidget(QWidget *parent = NULL, Qt::WindowFlags f = 0);

private slots:

    /**
     * @brief Updates the GUI when the underlying MediaObject changes states
     */
    void mediaStateChanged(Phonon::State newState, Phonon::State oldState);

    /**
     * @brief Plays or pauses the media, depending on current state
     */
    void playPause();

private:
    Phonon::MediaObject *m_media;
    QPushButton* m_playButton;
    QPushButton* m_stopButton;
};

#endif // CAPTURE_H
