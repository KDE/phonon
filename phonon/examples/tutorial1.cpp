/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without fee is hereby
    granted, provided that the above copyright notice appear in all
    copies and that both that the copyright notice and this
    permission notice and warranty disclaimer appear in supporting
    documentation, and that the name of the author not be used in
    advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.

    The author disclaim all warranties with regard to this
    software, including all implied warranties of merchantability
    and fitness.  In no event shall the author be liable for any
    special, indirect or consequential damages or any damages
    whatsoever resulting from loss of use, data or profits, whether
    in an action of contract, negligence or other tortious action,
    arising out of or in connection with the use or performance of
    this software.

*/

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    if (app.arguments().size() != 2) {
        return -1;
    }
    QCoreApplication::setApplicationName("Phonon Tutorial 1");
    Phonon::MediaObject media;
    media.setCurrentSource(app.arguments().at(1));
    Phonon::AudioOutput output;
    Phonon::createPath(&media, &output);
    QObject::connect(&media, SIGNAL(finished()), &app, SLOT(quit()));
    media.play();

    return app.exec();
}
