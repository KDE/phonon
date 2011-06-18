import Qt 4.7
import Phonon 1.0

Rectangle {
    width: 320
    height: 240
    color: "black"

    Media {
        id: media
        source: "video.ogv"

        AudioOutput {
            id: audio
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
            }
        }
    }
}
