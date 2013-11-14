/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011-2013 Harald Sitter <sitter@kde.org>

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

#include "factory_p.h"

#include "backendinterface.h"
#include "globalstatic_p.h"
#include "phononnamespace_p.h"
#include "phononversion.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

namespace Phonon {

class FactoryPrivate
{
public:
    FactoryPrivate();
    ~FactoryPrivate();

    bool createBackend();

    QObject *backendObject;
    BackendInterface *interface;
};

PHONON_GLOBAL_STATIC(Phonon::FactoryPrivate, globalFactory)

static inline void ensureLibraryPathSet()
{
#ifdef PHONON_LIBRARY_PATH
    static bool done = false;
    if (!done) {
        done = true;
        QCoreApplication::addLibraryPath(QLatin1String(PHONON_LIBRARY_PATH));
    }
#endif // PHONON_LIBRARY_PATH
}

bool FactoryPrivate::createBackend()
{
    pDebug() << Q_FUNC_INFO << "Phonon" << PHONON_VERSION_STR << "trying to create backend...";
#ifndef QT_NO_LIBRARY
    Q_ASSERT(backendObject == 0);

    // If the user defines a backend with PHONON_BACKEND this overrides the
    // platform plugin (because we cannot influence its lookup priority) and
    // consequently will try to find/load the defined backend manually.
    const QByteArray backendEnv = qgetenv("PHONON_BACKEND");

    if (!backendObject) {
        ensureLibraryPathSet();

        // could not load a backend through the platform plugin. Falling back to the default
        // (finding the first loadable backend).
        const QLatin1String suffix("/phonon_backend/");
        const QStringList paths = QCoreApplication::libraryPaths();
        for (int i = 0; i < paths.count(); ++i) {
            const QString libPath = paths.at(i) + suffix;
            const QDir dir(libPath);
            if (!dir.exists()) {
                pDebug() << Q_FUNC_INFO << dir.absolutePath() << "does not exist";
                continue;
            }

            QStringList plugins(dir.entryList(QDir::Files));

            if (!backendEnv.isEmpty()) {
                pDebug() << "trying to load:" << backendEnv << "as first choice";
                const int backendIndex = plugins.indexOf(QRegExp(backendEnv + ".*"));
                if (backendIndex != -1)
                    plugins.move(backendIndex, 0);
            }

            foreach (const QString &plugin, plugins) {
                QPluginLoader pluginLoader(libPath + plugin);
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  load failed:"
                             << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                backendObject = pluginLoader.instance();
                if (backendObject) {
                    break;
                }

                // no backend found, don't leave an unused plugin in memory
                pluginLoader.unload();
            }

            if (backendObject) {
                break;
            }
        }
        if (!backendObject) {
            pWarning() << Q_FUNC_INFO << "phonon backend plugin could not be loaded";
            return false;
        }
    }

    pDebug() << Q_FUNC_INFO
             << "Phonon backend"
             << backendObject->property("backendName").toString()
             << "version"
             << backendObject->property("backendVersion").toString()
             << "loaded";

    return true;
#else //QT_NO_LIBRARY
    pWarning() << Q_FUNC_INFO << "Trying to use Phonon with QT_NO_LIBRARY defined. "
                                 "That is currently not supported";
    return false;
#endif
}

FactoryPrivate::FactoryPrivate()
    : backendObject(0)
    , interface(0)
{
    // Add the post routine to make sure that all other global statics (especially the ones from Qt)
    // are still available. If the FactoryPrivate dtor is called too late many bad things can happen
    // as the whole backend might still be alive.
    qAddPostRoutine(globalFactory.destroy);
}

FactoryPrivate::~FactoryPrivate()
{
    delete backendObject;
}

QObject *Factory::createPlayer(QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::PlayerClass, parent);
}

QObject *Factory::createEffect(int effectId, QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::EffectClass, parent, QList<QVariant>() << effectId);
}

QObject *Factory::createVolumeFaderEffect(QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::VolumeFaderEffectClass, parent);
}

QObject *Factory::createAudioOutput(QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::AudioOutputClass, parent);
}

QObject *Factory::createVideoWidget(QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::VideoWidgetClass, parent);
}

QObject *Factory::createAudioDataOutput(QObject *parent)
{
    if (!backend())
        return 0;
    return interface()->createObject(BackendInterface::AudioDataOutputClass, parent);
}

QObject *Factory::backend()
{
    if (globalFactory.isDestroyed())
        return 0;

    if (globalFactory->backendObject == 0) {
        globalFactory->createBackend();
        if (globalFactory->backendObject)
            globalFactory->interface = qobject_cast<BackendInterface *>(globalFactory->backendObject);
    }

    return globalFactory->backendObject;
}

BackendInterface *Factory::interface()
{
    if (globalFactory.isDestroyed())
        return 0;

    return globalFactory->interface;
}

} // namespace Phonon
