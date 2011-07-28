import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0
import Phonon 1.0

Rectangle {
    id: browser

    anchors.fill: parent
    color: palette.window

    SystemPalette { id: palette }

    BorderImage {
        id: titleBar

        property int heightOffset: 7

        source: "titlebar.sci";
        width: parent.width;
        height: 52

        Item {
            width: parent.width
            height: parent.height - parent.heightOffset

            Rectangle {
                id: upButton

                width: 48
                height: parent.height
                color: "transparent"

                Image { anchors.centerIn: parent; source: "go-up.png" }

                MouseArea {
                    id: upArea;
                    anchors.fill: parent
                    onClicked: {
                        if (foldermodel.parentFolder != "")
                            foldermodel.folder = foldermodel.parentFolder
                    }
                }

                states: [
                    State {
                        name: "pressed"
                        when: upArea.pressed
                        PropertyChanges { target: upButton; color: palette.highlight }
                    }
                ]
            }

            // visual separator
            Rectangle {
                id: titleSeperator
                color: "gray"
                width: 1
                height: parent.height
                anchors.left: upButton.right
            }

            Text {
                anchors {
                    left: titleSeperator.right; right: parent.right;
                    leftMargin: 4; rightMargin: 4
                    verticalCenter: parent.verticalCenter
                }
                text: foldermodel.folder
                color: "white"
                elide: Text.ElideLeft; horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter
                font.pixelSize: 32
            }
        }
    }

    FolderListModel {
        id: foldermodel
        folder: "/home/me/Videos"
    }

    Component {
        id: filedelegate

        Item {
            id: fileDelegateItem
            height: 52
            width: browser.width

            function exec() {
                if (foldermodel.isFolder(index))
                    foldermodel.folder = filePath;
                else
                    master.play(filePath);
            }

            Row {
                spacing: 5

                Image {
                    width: 48; height: 48;
                    source: (foldermodel.isFolder(index)) ? "folder-grey.png" : ""
                }

                Media {
                    id: media
                    source: filePath

                    onStateChanged: {
//                        if (paused)
//                            play()
                        if (stopped)
                            play()
                    }

                    Video {
                        width: 48; height: 48
                    }

                    onVisibleChanged: {
                        if (!visible)
                            pause()
                        else
                            play()
                    }
                }

                Text {
                    id: nameText

                    text: fileName
                    font.pixelSize: 32
                    color: palette.windowText
                }
            }

            MouseArea {
                id: mouseRegion
                anchors.fill: parent
                onClicked: exec()
            }
        }
    }

    ListView {
        id: view

        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        width: parent.width

        model: foldermodel
        delegate: filedelegate
    }
}
