/*
videocapturetestapp.cpp - main() function

Copyright (c) 2005-2008 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "mainwindow.h"

#define VIDEOCAPTURETESTAPP_VERSION "0.1"

int main( int argc, char *argv[] )
{
    KAboutData about_data( "videocapturetestapp", 0,
        ki18n( "videocapturetestapp" ), VIDEOCAPTURETESTAPP_VERSION,
        ki18n( "Video Capture Test Application." ),
        KAboutData::License_GPL,
        ki18n( "Copyright 2005-2008 Cláudio da Silveira Pinheiro" ), ki18n( "taupter@gmail.com" ) );

    KCmdLineArgs::init( argc, argv, &about_data );

    KApplication app;

    MainWindow* window = new MainWindow();
    window->show();

    return app.exec();
}
