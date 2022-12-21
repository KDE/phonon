// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.18 as Kirigami
import org.kde.phonon 4.13 as Phonon

Kirigami.ApplicationWindow {
    pageStack.initialPage: Kirigami.Page {
        ColumnLayout {
            anchors.fill: parent
            Phonon.AudioOutput {
                id: audioOutput
            }
            Phonon.VideoItem {
                id: videoItem
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Phonon.MediaObject {
                id: player
            }
            Rectangle {
                color: "green"
                height: 12
                Layout.fillWidth: true
            }
        }

        Component.onCompleted: {
            Phonon.Hack.connect(player, audioOutput)
            Phonon.Hack.connect(player, videoItem.output)
            Phonon.Hack.setCurrentSource(player, "file:///home/me/Public/EVO.mkv")
            player.play()
        }
    }
}
