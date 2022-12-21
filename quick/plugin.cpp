// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "plugin.h"

#include <QtQml>

#include <mediaobject.h>
#include <audiooutput.h>

#include "videoitem.h"

class Hack : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_INVOKABLE void connect(QObject *source, QObject *sink)
    {
        qDebug() << source << sink;
        Phonon::createPath(dynamic_cast<Phonon::MediaNode *>(source), dynamic_cast<Phonon::MediaNode *>(sink));
    }

    Q_INVOKABLE void setCurrentSource(Phonon::MediaObject *mo, const QUrl &url)
    {
        mo->setCurrentSource(Phonon::MediaSource(url));
    }
};

void Plugin::registerTypes(const char* uri)
{
    qmlRegisterSingletonType<Hack>("org.kde.phonon", 4, 13, "Hack", [](QQmlEngine *engine, QJSEngine *jEngine){ return new Hack;});
    qmlRegisterType<Phonon::MediaObject>("org.kde.phonon", 4, 13, "MediaObject");
    qmlRegisterType<Phonon::AudioOutput>("org.kde.phonon", 4, 13, "AudioOutput");
    qmlRegisterType<VideoItem>("org.kde.phonon", 4, 13, "VideoItem");
}

#include "plugin.moc"
