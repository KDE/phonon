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

/*
 * This demo uses the AVCapture class from Phonon to capture video and audio
 * at the same time. The object of this class is connected to a VideoWidget and
 * an AudioOutput.
 * 
 * If you only want to capture video, or only audio, see the first capture demo.
 * 
 * Both the video capture device and the audio capture device can be selected from
 * their own combo boxes. If you have a device that supports both video and audio
 * capture, it should show in both combo boxes.
 */

#include "captureapp2.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Phonon AVCapture Demo");

    MediaPlayer mp(NULL);
    mp.setWindowTitle("Phonon AVCapture Demo");
    mp.show();

    return app.exec();
}
