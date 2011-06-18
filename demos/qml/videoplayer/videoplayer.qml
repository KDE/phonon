import Qt 4.7
import Phonon 1.0

Rectangle {
    width: 320
    height: 240
    color: "black"

    Media {
        id: media
        source: "video.ogv"

        onStateChanged: {
            if (playing)
                playPause.state = "playing"
            else
                playPause.state = "paused"
        }

        AudioOutput {
            id: audio
            onVolumeChanged: { volumeSlider.value = volume * 100.0 }
        }

        Video {
            id: video
            width: 320
            height: 240
        }
    }

    Image {
        id: controls
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        source: "controls-background.png"
        opacity: 0.788235294118

        Row {
            anchors.top: parent.top
            anchors.horizontalCenter: controls.horizontalCenter
            anchors.topMargin: 10

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
            }
        }
    }
}
