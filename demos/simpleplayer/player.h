/*  This file is part of the KDE project
    Copyright (C) 2010 Trever Fischer <tdfischer@fedoraproject.org>

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
#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <phonon/Global>

class QPushButton;
namespace Phonon {
    class MediaObject;
    class Mrl;
}

/**
 * @brief A simple media player widget
 */
class Player : public QWidget {

Q_OBJECT

public:
    Player(QWidget *parent = 0, Qt::WindowFlags flag = 0);

public slots:

    /**
     * @brief Load a specific URL
     */
    void load(const Phonon::Mrl &url);

    /**
     * @brief Prompt the user for a file to load
     */
    void load();

private slots:

    /**
     * @brief Updates the GUI when the underlying MediaObject changes states
     */
    void mediaStateChanged(Phonon::State newState, Phonon::State oldState);

    /**
     * @brief Plays or pauses the media, depending on current state
     */
    void playPause();

private:
    Phonon::MediaObject *m_media;
    QPushButton *m_playPause;
    QPushButton *m_stop;

};

#endif //PLAYER_H
