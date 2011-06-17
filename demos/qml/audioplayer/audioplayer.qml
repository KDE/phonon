import Qt 4.7
import Phonon 1.0

Rectangle {
    width: 64
    height:  64
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
