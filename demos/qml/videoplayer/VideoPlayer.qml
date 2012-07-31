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
import QtDesktop 0.1
import Phonon 1.0

Rectangle {
    id: player

    property bool wasVisibleOnce: false

    function play(path) {
        focus = true
        media.source = path
        media.play()
    }

    function stop() {
        media.stop()
    }

    width: 800
    height: 480
    color: "black"
    focus: true

    Timer {
        function reset() {
            restart()
            video.cursorVisible = true
            controls.showControls = true
        }

        function undo() {
            stop()
            video.cursorVisible = true
            controls.showControls = true
        }

        id: movementTimer
        running: true
        interval: 2000
        onTriggered:{
            video.cursorVisible = false
            controls.showControls = false
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onMousePositionChanged: movementTimer.reset()
        onEntered: movementTimer.start()
        onExited: movementTimer.undo()
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Space)
            media.togglePlay()
    }

    onVisibleChanged: {
        if (wasVisibleOnce && !visible)
            video.fullScreen = false
        if (!wasVisibleOnce && visible)
            wasVisibleOnce = true
    }

    Media {
        id: media

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
        }

        Video {
            id: video
            width: player.width
            height: player.height
            opacity: 0

            ContextMenu {
                id: videoContext
                MenuItem { text: "Bling"; enabled: false }
                MenuItem {
                    text: "Meta Data"
                    onSelected: player.state = 'metaDataDisplay'
                }
            }

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: video.fullScreen = !video.fullScreen
            }

            MouseArea {
                anchors.fill:  parent
                acceptedButtons: Qt.RightButton
                onPressed: videoContext.showPopup(mouseX, mouseY)
            }

            states: [
                State {
                    name: "playing"
                    // Must not change state on pause.
                    when: media.playing || media.paused
                    PropertyChanges { target: video; opacity: 1 }
                }
            ]

            transitions: Transition {
                from: ""
                to: "playing"
                reversible: true
                SequentialAnimation {
                    PropertyAnimation {
                        target: video;
                        properties: "opacity"
                        easing.type: Easing.InOutExpo;
                        duration: 2000
                    }
                }
            }
        }
    }

    Image {
        id: controls

        property bool showControls: true

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        source: "controls-background.png"
        opacity: 0.788235294118

        Image {
            anchors { top: parent.top; topMargin: 15; left: parent.left; leftMargin: 15 }
            source: "fullscreen-button.png"

            MouseArea {
                anchors.fill: parent
                onClicked: video.fullScreen = !video.fullScreen
            }
        }

        Row {
            anchors { top: parent.top; topMargin: 15; horizontalCenter: controls.horizontalCenter }
            spacing: 30

            Image { source: "backward-button.png" }

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

            Image { source: "forward-button.png" }
        }

        Slider {
            id: volumeSlider

            width: 136
            height: 19
            anchors { top: parent.top; topMargin: 20; right: parent.right; rightMargin: 20 }
            updateValueWhileDragging: true
            groove: Image { source: "volume-bar.png" }
            handle: Image { source: "volume-slider.png" }

            value: Math.round(100 * audio.volume)
            minimumValue: 0
            maximumValue: 100

            Binding {
                when: volumeSlider.pressed
                target: audio
                property: "volume"
                value: volumeSlider.value * 0.01
            }

            Binding {
                when: !volumeSlider.pressed
                target: parent
                property: "value"
                value: Math.round(100 * audio.volume)
            }

            WheelArea {
                anchors.fill: parent
                verticalMinimumValue: parent.minimumValue
                verticalMaximumValue: parent.maximumValue
                property double step: (parent.maximumValue - parent.minimumValue)/100

                onVerticalWheelMoved: audio.volume += ((verticalDelta/4*step) * 0.01)
            }
        }

        Slider {
            id: progressSlider

            enabled: media.seekable
            width: 749
            height: 19
            anchors { bottom: timeIndicator.top; bottomMargin: 5; horizontalCenter: parent.horizontalCenter }
            updateValueWhileDragging: true
            groove: Image { source: "progress-bar.png" }
            handle: Image { source: "volume-slider.png" }

            value: media.time
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
                verticalMinimumValue: parent.minimumValue
                verticalMaximumValue: parent.maximumValue
                property double step: (parent.maximumValue - parent.minimumValue)/100

                onVerticalWheelMoved: media.time += (verticalDelta/4*step)
            }
        }

        Item {
            id: timeIndicator

            anchors {
                left: progressSlider.left; leftMargin: 5
                right: progressSlider.right; rightMargin: 5
                bottom: parent.bottom; bottomMargin: 20
            }

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

        states: State {
            name: "hide"; when: controls.showControls == false
            PropertyChanges { target: controls; opacity: 0 }
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
    }

    Rectangle {
        id: metaData
        x: 300
        y: 140
        width: 200
        height: 200
        color: "#ffffff"
        visible: false

        Column {
            anchors.fill: parent
            spacing: 16

            Text { text: "Artist: " + media.metaData.artist; font.pixelSize: 12 }
            Text { text: "Title: " + media.metaData.title; font.pixelSize: 12 }
            Text { text: "Album: " + media.metaData.album; font.pixelSize: 12 }
        }

        Button {
            text: "Roger!"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            onClicked: player.state = ''
        }
    }

    states: State { name: "metaDataDisplay"; PropertyChanges { target: metaData; visible: true } }
}
