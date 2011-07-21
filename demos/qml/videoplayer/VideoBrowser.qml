import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0

ListView {
    FolderListModel {
        id: foldermodel
        folder: "/home/me/Videos"
        showDirs: false
    }

    Component {
        id: filedelegate

        Text {
            text: fileName

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    master.play(filePath)
                }
            }
        }
    }

    model: foldermodel
    delegate: filedelegate
}
