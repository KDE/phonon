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
        id: icon
        anchors.centerIn: parent
        width: 32;
        height: 32;
        source: "media-playback-start.png"

        MouseArea {
            anchors.fill: parent
            onPressed: {
                if (media.playing)
                    media.stop()
                else
                    media.play()
                icon.visible = false
            }
        }
    }
}
