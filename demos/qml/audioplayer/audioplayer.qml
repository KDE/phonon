/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

import Qt 4.7
import Phonon 1.0

Rectangle {
    width: 64
    height: 64
    color: "black"

    Image {
        id: icon
        anchors.centerIn: parent
        width: 32;
        height: 32;
        source: "media-playback-start.png"

        Media {
            id: mediaElement
            source: "sound.wav"

            onPlayingChanged: {
                if (!playing) {
                    stop()
                    icon.source = "media-playback-start.png"
                } else {
                    icon.source = "media-playback-stop.png"
                }
            }

            AudioOutput {
                id: audioOutput
                volume: -10
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (mediaElement.playing)
                    mediaElement.stop()
                else
                    mediaElement.play()
            }
        }
    }
}
