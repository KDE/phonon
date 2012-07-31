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

import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0
import Phonon 1.0

Rectangle {
    id: browser

    anchors.fill: parent
    color: palette.window

    SystemPalette { id: palette }


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

            Rectangle {
                id: highlight; visible: false
                anchors.fill: parent
                color: palette.highlight
                gradient: Gradient {
                    GradientStop { id: t1; position: 0.0; color: palette.highlight }
                    GradientStop { id: t2; position: 1.0; color: Qt.lighter(palette.highlight) }
                }
            }

            Row {
                spacing: 5

                Image {
                    width: 48; height: 48;
                    source: (foldermodel.isFolder(index)) ? "folder-grey.png" : ""
                }

                Media {
                    id: media
                    source: (shouldPreview()) ? filePath : ""
                    visible: shouldPreview()

                    function shouldPreview() {
                        if (foldermodel.isFolder(index))
                            return false;
                        var path = filePath + '' // Apparently filePath is no String.
                        var suffix = path.substring(path.lastIndexOf(".")+1)
                        if (suffix === "avi" ||
                                suffix === "mkv" ||
                                suffix === "mp4" ||
                                suffix === "webm" ||
                                suffix === "ogv" ||
                                suffix === "wmv" ||
                                suffix === "mov")
                            return true;
                        return false;
                    }

                    onSourceChanged: {
                        if (source != "")
                            play()
                    }

                    onStateChanged: {
                        if (!shouldPreview())
                            return;
//                        if (paused)
//                            play()
                        if (stopped)
                            play()
                    }

                    onVisibleChanged: {
                        if (!shouldPreview())
                            return;
                        if (!visible)
                            pause()
                        else
                            play()
                    }

                    Video {
                        width: 48; height: 48
                    }
                }

                Text {
                    id: nameText

                    text: fileName
                    font.pixelSize: 32
                    color: palette.windowText
                    elide: Text.ElideLeft; horizontalAlignment: Text.AlignRight; verticalAlignment: Text.AlignVCenter
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: exec()
            }
            states: [
                State {
                    name: "pressed"
                    when: mouseArea.pressed
                    PropertyChanges { target: highlight; visible: true }
                    PropertyChanges { target: nameText; color: palette.highlightedText }
                }
            ]
        }
    }

    ListView {
        id: view

        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        cacheBuffer: 520

        model: foldermodel
        delegate: filedelegate
    }


    BorderImage {
        id: titleBar

        property int heightOffset: 7

        source: "titlebar.sci";
        width: parent.width;
        anchors.top: parent.top
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

}
