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

#include <QApplication>

#include <phonon/Mrl>

#include "player.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    //On most platforms, this tells the audio system the name of the application responsible for which stream
    app.setApplicationName("simpleplayer");

    Player p;
    p.show();

    //Not passing an argument of what to play will prompt for a file inside Player::load(), called by Player::playPause()
    QStringList args = QApplication::arguments();
    if (args.size() > 1) {
        p.load(Phonon::Mrl(args[1]));
    }

    return app.exec();
}
