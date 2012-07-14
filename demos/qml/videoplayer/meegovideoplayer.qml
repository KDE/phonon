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

import QtQuick 1.0
import Phonon 1.0
import com.nokia.meego 1.0

PageStackWindow {
    id: player

    showStatusBar: false
    platformStyle: platformStyle.cornersVisible = false
    initialPage: mainPage

    Page {
        id: mainPage

        Media {
            id: media
            anchors.top: parent.top
            source: "video.ogv"

            onStateChanged: {
                if (playing)
                    playPause.state = "playing"
                else
                    playPause.state = "paused"
            }

            onTotalTimeChanged: { progressSlider.maximumValue = totalTime }
            onTimeChanged: { progressSlider.value = time }

            AudioOutput {
                id: audio
    //            onVolumeChanged: { volumeSlider.value = volume * 100.0 }
            }

            Video {
                id: video
                width: mainPage.width
                height: mainPage.height
            }
        }

        tools: ToolBarLayout {
            id: layout
            ToolIcon {
                id: playPause
                iconSource: "play-button.png"
                onClicked: {
                    if (playPause.state == 'playing') {
                        media.stop()
                        playPause.state = 'paused'
                    } else {
                        media.play()
                        playPause.state = 'playing'
                    }
                }
                states: [
                    State {
                        name: "paused"
                        PropertyChanges { target: playPause; iconSource: "play-button.png" }
                    },
                    State {
                        name: "playing"
                        PropertyChanges { target: playPause; iconSource: "pause-button.png" }
                    }
                ]
            }
            Slider {
                id: progressSlider
                width: parent.width - playPause.width;
                onPressedChanged: {
                    if (pressed) return
                    media.time = value
                }
            }
        }
    }
}
