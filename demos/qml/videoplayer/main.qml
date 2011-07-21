import QtQuick 1.0

Rectangle {
    id: master
    width: 800
    height: 480

    state: "Browsing"

    function play(path) {
        player.play(path)
        master.state = "Playing"
    }

    VideoBrowser {
        id : browser
        anchors.fill: parent
    }

    VideoPlayer {
        id: player
        anchors.fill: parent

        MouseArea {
            id: mousecontrol
            anchors.fill: parent
            acceptedButtons: Qt.MiddleButton
            onPressed: {
                if (mouse.button == Qt.MiddleButton) {
                    mouse.accepted = true
                    player.stop()
                    master.state = "Browsing"
                } else
                    mouse.accepted = false
            }
        }
    }

    states: [
        State {
            name: "Browsing"
            PropertyChanges { target: browser; visible: true }
            PropertyChanges { target: player; visible: false }
        },
        State {
            name: "Playing"
            PropertyChanges { target: browser; visible: false; opacity: 0 }
            PropertyChanges { target: player; visible: true }
        }
    ]
}
