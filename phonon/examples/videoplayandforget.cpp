/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <phonon/ui/videoplayer.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kurl.h>
#include <klocale.h>

using namespace Phonon;

int main(int argc, char ** argv)
{
    KCmdLineOptions options;
    options.add("+URL", ki18n("An URL to a video"));

    KAboutData about("videoplayandforget", 0, ki18n("Phonon VideoPlayer Example"),
            "1.0", KLocalizedString(),
            KAboutData::License_LGPL);
    about.addAuthor(ki18n("Matthias Kretz"), KLocalizedString(), "kretz@kde.org");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;
    KUrl url;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() == 1)
    {
        url = args->url(0);
        if (url.isValid())
        {
            VideoPlayer player(Phonon::VideoCategory);
            QObject::connect(&player, SIGNAL(finished()), &app, SLOT(quit()));
            player.show();
            player.resize(640, 480);
            player.play(url);
            player.seek(player.totalTime() * 9 / 10);
            return app.exec();
        }
    }
    return 1;
}
