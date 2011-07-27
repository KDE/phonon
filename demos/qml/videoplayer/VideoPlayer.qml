import Qt 4.7
import QtDesktop 0.1
import Phonon 1.0

Rectangle {
    id: player
    width: 800
    height: 480
    color: "black"

    property bool wasVisibleOnce: false

    function play(path) {
        focus = true
        media.source = path
        media.play()
    }

    function stop() {
        media.stop()
    }

    Timer {
        id: movementTimer
        running: true
        interval: 2000
        onTriggered:{
            video.cursorVisible = false
            controls.showControls = false
        }
    }

    MouseArea
    {
        anchors.fill: parent
        hoverEnabled: true
        onMousePositionChanged: {
            movementTimer.restart()
            video.cursorVisible = true
            controls.showControls = true
        }
    }

    focus: true
    Keys.onPressed: {
        if (event.key == Qt.Key_Space)
            media.togglePlay()
    }

    onVisibleChanged: {
        if (wasVisibleOnce && !visible)
            video.fullScreen = false
        if (!wasVisibleOnce && visible )
            wasVisibleOnce = true
    }

    Media {
        id: media
        source: "video.ogv"

        property string timeString: parseTime(time)
        property string remainingTimeString: "-" + parseTime(totalTime - time)
        property string totalTimeString: parseTime(totalTime)

        function togglePlay() {
            (media.playing) ? media.pause() : media.play()
        }

        function padIntString(number) {
            var str = '' + number;
            if (str.length < 2)
                str = '0' + str;
            return str;
        }

        function parseTime(ms) {
            // Do not display hour unless there is anything to show.
            // Always show minutes and seconds though.
            // Rationale: plenty of videos are <1h but >1m
            var showHour = true
            if (totalTime < 3600000)
                showHour = false

            var s = Math.floor(ms / 1000)
            var m = Math.floor( s / 60)
            if (showHour)
                var h = Math.floor(m / 60)

            var time = '';
            time = padIntString(s % 60)
            time = padIntString(m % 60) + ':' + time
            if (showHour)
                // Do not pad hour as it looks ugly, also hour can exceed 24 anyway.
                time = h + ':' + time
            return time
        }

        AudioOutput {
            id: audio
            onVolumeChanged: { volumeSlider.value = volume * 100.0 }
        }

        Video {
            id: video
            width: player.width
            height: player.height

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: video.fullScreen = !video.fullScreen
            }
        }
    }

    Image {
        id: controls
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        source: "controls-background.png"
        opacity: 0.788235294118

        property bool showControls: true

        states: State {
            name: "hide"; when: controls.showControls == false
            PropertyChanges {
                target: controls;
                opacity: 0
            }
        }


        transitions: Transition {
            from: ""
            to: "hide"
            reversible: true
            SequentialAnimation {
                PropertyAnimation {
                    target: controls;
                    properties: "opacity"
                    easing.type: Easing.InOutExpo;
                    duration: 500
                }
            }
        }

        Image {
            source: "fullscreen-button.png"

            anchors.top: parent.top
            anchors.topMargin: 15
            anchors.left: parent.left
            anchors.leftMargin: 15

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    video.fullScreen = !video.fullScreen
                }
            }
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
                    onClicked: media.togglePlay()
                }

                states: [
                    State {
                        when: !media.playing
                        name: "paused"
                        PropertyChanges { target: playPause; source: "play-button.png" }
                    },
                    State {
                        when: media.playing
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

        Slider {
            id: progressSlider
            enabled: media.seekable
            width: 749
            height: 19
            anchors.bottom: timeIndicator.top
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            updateValueWhileDragging: true
            groove: Image { source: "progress-bar.png" }
            handle: Image { source: "volume-slider.png" }

            minimumValue: 0
            maximumValue: media.totalTime

            Binding {
                when: progressSlider.pressed
                target: media
                property: "time"
                value: progressSlider.value
            }

            Binding {
                when: !progressSlider.pressed
                target: parent
                property: "value"
                value: media.time
            }

            WheelArea {
                anchors.fill: parent
                horizontalMinimumValue: parent.minimumValue
                horizontalMaximumValue: parent.maximumValue
                property double step: (parent.maximumValue - parent.minimumValue)/100

                onHorizontalWheelMoved: {
                    media.time += horizontalDelta/4*step
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
                text: media.timeString
                color: "white"
                font.bold: true
            }

            Text {
                anchors.right: parent.right
                text: media.remainingTimeString
                color: "white"
                font.bold: true
            }
        }
    }
}
