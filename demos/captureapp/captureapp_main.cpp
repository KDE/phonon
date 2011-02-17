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
 * This demo tries to capture from a single device, video or audio.
 * Video and audio is not captured at the same time.
 * 
 * It shows how to use Phonon's MediaSource, VideoCaptureDevice and AudioCaptureDevice
 * classes to capture video or audio. Phonon's capture device models are used to
 * populate the combo box that selects the device.
 */

#include "captureapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Phonon Capture Demo Application");

    MediaPlayer mp;
    mp.setWindowTitle("Phonon Capture Demo Application");
    mp.show();

    return app.exec();
}
