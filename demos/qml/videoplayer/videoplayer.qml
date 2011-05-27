import Qt 4.7
import Phonon 1.0

Rectangle {
    width: 320
    height: 240
    color: "black"

    Video {
        id: video
        source: "video.ogv"
        width: 320
        height: 240


//        onPlayingChanged: {
//            if (!playing) {
//                stop()
//                icon.source = "media-playback-start.png"
//            } else {
//                icon.source = "media-playback-stop.png"
//            }
//        }
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
                if (video.playing)
                    video.stop()
                else
                    video.play()
                icon.visible = false
            }
        }
    }
}
