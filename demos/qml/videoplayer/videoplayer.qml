import Qt 4.7
import Phonon 1.0

Rectangle {
    id: player
    width: 800
    height: 480
    color: "black"

    Timer {
        id: movementTimer
        running: true
        interval: 2000
        onTriggered:{
            controls.state = "inactive"
        }
    }

    MouseArea
    {
        anchors.fill: parent
        hoverEnabled: true
        onMousePositionChanged: {
            movementTimer.restart()
            controls.state = "active"
        }
    }

    Media {
        id: media
        source: "video.ogv"

        onStateChanged: {
            if (playing)
                playPause.state = "playing"
            else
                playPause.state = "paused"
        }

        onTotalTimeChanged: { progressSlider.max = totalTime }
        onTimeChanged: { progressSlider.value = time }

        AudioOutput {
            id: audio
            onVolumeChanged: { volumeSlider.value = volume * 100.0 }
        }

        Video {
            id: video
            width: player.width
            height: player.height
        }
    }

    Image {
        id: controls
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        source: "controls-background.png"
        opacity: 0.788235294118

        states: [
            State {
                name: "active"
                PropertyChanges { target: controls; visible: true }
            },
            State {
                name: "inactive"
                PropertyChanges { target: controls; visible: false }
            }
        ]

        Image {
            source: "fullscreen-button.png"

            anchors.top: parent.top
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.leftMargin: 15
        }

        Row {
            anchors.top: parent.top
            anchors.horizontalCenter: controls.horizontalCenter
            anchors.topMargin: 15
            spacing: 30

            Image {
                source: "backward-button.png"
            }

            Image {
                id: playPause

                source: "play-button.png"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (playPause.state == 'playing') {
                            media.stop()
                            playPause.state = 'paused'
                        } else {
                            media.play()
                            playPause.state = 'playing'
                        }
                    }
                }

                states: [
                    State {
                        name: "paused"
                        PropertyChanges { target: playPause; source: "play-button.png" }
                    },
                    State {
                        name: "playing"
                        PropertyChanges { target: playPause; source: "pause-button.png" }
                    }
                ]
            }

            Image {
                source: "forward-button.png"
            }
        }

        Image {
            id: volumeSlider
            width: 136
            height: 19

            property real value
            onValueChanged: {
                // Ignore value changes whiles drag is in progress as to avoid
                // conficting information
                if (!volumeMouseArea.drag.active)
                    volumeHandle.x = volumeMouseArea.drag.maximumX * value / 100.0;
            }

            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            source: "volume-bar.png"

            Image {
                id: volumeHandle
                width: 27
                height: 28
                anchors.top: parent.top
                anchors.topMargin: -5
                source: "volume-slider.png"

                MouseArea {
                    id: volumeMouseArea
                    anchors.fill: parent
                    drag.target: volumeHandle
                    drag.axis: Drag.XAxis
                    drag.minimumX: 0
                    drag.maximumX: volumeSlider.width - volumeHandle.width
                    onPositionChanged: {
                        volumeSlider.value = drag.target.x * 100.0 / drag.maximumX
                        audio.volume = volumeSlider.value / 100.0
                    }
                }
            }
        }

        Image {
            id: progressSlider
            width: 749
            height: 19

            property real value
            property real max

            onValueChanged: {
                // Ignore value changes whiles drag is in progress as to avoid
                // conficting information
                if (!progressMouseArea.drag.active)
                    progressHandle.x = value * progressMouseArea.drag.maximumX / max;
            }

            anchors.bottom: timeIndicator.top
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            source: "progress-bar.png"

            Image {
                id: progressHandle
                width: 27
                height: 28
                anchors.top: parent.top
                anchors.topMargin: -5
                source: "volume-slider.png"

                property real cachedPos

                MouseArea {
                    id: progressMouseArea
                    anchors.fill: parent
                    drag.target: progressHandle
                    drag.axis: Drag.XAxis
                    drag.minimumX: 0
                    drag.maximumX: progressSlider.width - progressHandle.width

                    onReleased: {
                        media.time = progressSlider.max * progressHandle.x / drag.maximumX;
                    }
                }
            }
        }

        Item {
            id: timeIndicator
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.right: progressSlider.right
            anchors.rightMargin: 5
            anchors.left: progressSlider.left
            anchors.leftMargin: 5

            Text {
                anchors.left: parent.left
                text: "1.10.5"
                color: "white"
                font.bold: true
            }

            Text {
                anchors.right: parent.right
                text: "-0.4.5"
                color: "white"
                font.bold: true
            }
        }
    }
}
