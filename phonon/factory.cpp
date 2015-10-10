/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
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

#include "factory_p.h"

#include "backendinterface.h"
#include "medianode_p.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "globalstatic_p.h"
#include "objectdescription.h"
#include "platformplugin.h"
#include "phononconfig_p.h"
#include "phononnamespace_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#ifndef PHONON_NO_DBUS
#include <QtDBus/QtDBus>
#endif
#include <QApplication>
#include <QMessageBox>
#include <QString>

namespace Phonon
{

class PlatformPlugin;
class FactoryPrivate : public Phonon::Factory::Sender
{
    friend QObject *Factory::backend(bool);
    Q_OBJECT
    public:
        FactoryPrivate();
        ~FactoryPrivate();
        bool tryCreateBackend(const QString &path);
        // Implementation depends on Qt version.
        bool createSuitableBackend(const QString &libPath, const QList<QString> &plugins);
        bool createBackend();
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        PlatformPlugin *platformPlugin();

        PlatformPlugin *m_platformPlugin;
        bool m_noPlatformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        QPointer<QObject> m_backendObject;

        QList<QObject *> objects;
        QList<MediaNodePrivate *> mediaNodePrivateList;

    private Q_SLOTS:
        /**
         * This is called via DBUS when the user changes the Phonon Backend.
         */
#ifndef PHONON_NO_DBUS
        void phononBackendChanged();
#endif //PHONON_NO_DBUS

        /**
         * unregisters the backend object
         */
        void objectDestroyed(QObject *);

        void objectDescriptionChanged(ObjectDescriptionType);
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

void Factory::setBackend(QObject *b)
{
    Q_ASSERT(globalFactory->m_backendObject == 0);
    globalFactory->m_backendObject = b;
}

bool FactoryPrivate::tryCreateBackend(const QString &path)
{
    QPluginLoader pluginLoader(path);

    pDebug() << "attempting to load" << path;
    if (!pluginLoader.load()) {
        pDebug() << Q_FUNC_INFO << "  load failed:" << pluginLoader.errorString();
        return false;
    }
    pDebug() << pluginLoader.instance();
    m_backendObject = pluginLoader.instance();
    if (m_backendObject) {
        return true;
    }

    // no backend found, don't leave an unused plugin in memory
    pluginLoader.unload();
    return false;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
struct BackendDescriptor {
    explicit BackendDescriptor(const QString &path)
        : isValid(false)
    {
        QPluginLoader loader(path);

        iid = loader.metaData().value(QLatin1String("IID")).toString();

        const QJsonObject metaData = loader.metaData().value(QLatin1String("MetaData")).toObject();
        name = metaData.value(QLatin1String("Name")).toString();
        icon = metaData.value(QLatin1String("Icon")).toString();
        version = metaData.value(QLatin1String("Version")).toString();
        website = metaData.value(QLatin1String("Website")).toString();
        preference = metaData.value(QLatin1String("InitialPreference")).toDouble();

        pluginPath = path;

        if (name.isEmpty())
            name = QFileInfo(path).baseName();

        if (iid.isEmpty())
            return; // Not valid.

        isValid = true;
    }

    bool isValid;

    QString iid;

    QString name;
    QString icon;
    QString version;
    QString website;
    int preference;

    QString pluginPath;

    /** Implemented for qSort(QList) */
    bool operator <(const BackendDescriptor &rhs) const
    {
        return this->preference < rhs.preference;
    }
};

bool FactoryPrivate::createSuitableBackend(const QString &libPath, const QList<QString> &plugins)
{
    // User configured preference.
    QList<QString> iidPreference;
    QSettings settings("kde.org", "libphonon");
    const int size = settings.beginReadArray("Backends");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        iidPreference.append(settings.value("iid").toString());
    }
    settings.endArray();

    // Find physical backend plugins.
    QList<BackendDescriptor> foundBackends;
    foreach (const QString &plugin, plugins) {
        BackendDescriptor descriptor(libPath + plugin);
        if (!descriptor.isValid)
            continue;
        foundBackends.append(descriptor);
    }
    qSort(foundBackends);

    // Try to pick a preferred backend.
    foreach (const QString &iid, iidPreference) {
        // This is slightly inefficient but we only have 2 backends :P
        // Also using a list requires less overall code than QMap<iid,desc>.
        QList<BackendDescriptor>::iterator it;
        for (it = foundBackends.begin(); it != foundBackends.end(); ++it) {
            const BackendDescriptor &descriptor = *it;
            if (descriptor.iid != iid) {
                continue;
            }
            if (tryCreateBackend(descriptor.pluginPath)) {
                return true;
            } else { // Drop backends that failed to construct.
                it = foundBackends.erase(it);
                if (it == foundBackends.end())
                    break;
            }
        }
    }

    // No Preferred backend could be constructed. Try all remaining backends
    // in order of initial preference.
    // Note that unconstructable backends have been dropped previously.
    foreach (const BackendDescriptor &descriptor, foundBackends) {
        if (tryCreateBackend(descriptor.pluginPath))
            return true;
    }
    return false;
}
#else // Qt 4
bool FactoryPrivate::createSuitableBackend(const QString &libPath, const QList<QString> &plugins)
{
    foreach (const QString &plugin, plugins) {
        if (tryCreateBackend(libPath + plugin))
            return true;
    }
    return false;
}
#endif

// This entire function is so terrible to read I hope it implodes some day.
bool FactoryPrivate::createBackend()
{
    pDebug() << Q_FUNC_INFO << "Phonon" << PHONON_VERSION_STR << "trying to create backend...";
#ifndef QT_NO_LIBRARY
    Q_ASSERT(m_backendObject == 0);

    // If the user defines a backend with PHONON_BACKEND this overrides the
    // platform plugin (because we cannot influence its lookup priority) and
    // consequently will try to find/load the defined backend manually.
    const QByteArray backendEnv = qgetenv("PHONON_BACKEND");

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f && backendEnv.isEmpty()) {
        // TODO: it would be very groovy if we could add a param, so that the
        // platform could also try to load the defined backend as preferred choice.
        m_backendObject = f->createBackend();
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    if (!m_backendObject) {
        ensureLibraryPathSet();

        // could not load a backend through the platform plugin. Falling back to the default
        // (finding the first loadable backend).
        const QStringList paths = QCoreApplication::libraryPaths();
        for (int i = 0; i < paths.count(); ++i) {
#ifdef __GNUC__
#warning this actually breaks with qmake....
#endif
#ifndef PHONON_BACKEND_DIR_SUFFIX
#ifdef __GNUC__
#error PHONON_BACKEND_DIR_SUFFIX must be defined.
#endif
#endif
            const QString libPath = paths.at(i) + PHONON_BACKEND_DIR_SUFFIX;
            const QDir dir(libPath);
            if (!dir.exists()) {
                pDebug() << Q_FUNC_INFO << dir.absolutePath() << "does not exist";
                continue;
            }

            QStringList plugins(dir.entryList(QDir::Files));

#ifdef Q_OS_SYMBIAN
            /* On Symbian OS we might have two plugins, one which uses Symbian
             * MMF framework("mmf"), and one which uses Real Networks's
             * Helix("hxphonon"). We prefer the latter because it's more
             * sophisticated, so we make sure the Helix backend is attempted
             * to be loaded first, and the MMF backend is used for backup. */
            {
                const int helix = plugins.indexof(QLatin1String("hxphonon"));
                if (helix != -1)
                    plugins.move(helix, 0);
            }
#endif

            if (!backendEnv.isEmpty()) {
                pDebug() << "trying to load:" << backendEnv << "as first choice";
                const int backendIndex = plugins.indexOf(QRegExp(backendEnv + ".*"));
                if (backendIndex != -1)
                    plugins.move(backendIndex, 0);
            }

            // This function implements a very simple trial-and-error loader for
            // Qt 4 and on top of that a preference system for Qt 5. Therefore
            // in Qt 5 we have backend preference independent of a platform
            // plugin.
            createSuitableBackend(libPath, plugins);

            if (m_backendObject) {
                break;
            }
        }
        if (!m_backendObject) {
            pWarning() << Q_FUNC_INFO << "phonon backend plugin could not be loaded";
            return false;
        }
    }

    pDebug() << Q_FUNC_INFO
             << "Phonon backend"
             << m_backendObject->property("backendName").toString()
             << "version"
             << m_backendObject->property("backendVersion").toString()
             << "loaded";

    connect(m_backendObject, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
            SLOT(objectDescriptionChanged(ObjectDescriptionType)));

    return true;
#else //QT_NO_LIBRARY
    pWarning() << Q_FUNC_INFO << "Trying to use Phonon with QT_NO_LIBRARY defined. "
                                 "That is currently not supported";
    return false;
#endif
}

FactoryPrivate::FactoryPrivate()
    :
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    m_platformPlugin(0),
    m_noPlatformPlugin(false),
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    m_backendObject(0)
{
    // Add the post routine to make sure that all other global statics (especially the ones from Qt)
    // are still available. If the FactoryPrivate dtor is called too late many bad things can happen
    // as the whole backend might still be alive.
    qAddPostRoutine(globalFactory.destroy);
#ifndef PHONON_NO_DBUS
    QDBusConnection::sessionBus().connect(QString(), QString(), QLatin1String("org.kde.Phonon.Factory"),
        QLatin1String("phononBackendChanged"), this, SLOT(phononBackendChanged()));
#endif
}

FactoryPrivate::~FactoryPrivate()
{
    for (int i = 0; i < mediaNodePrivateList.count(); ++i) {
        mediaNodePrivateList.at(i)->deleteBackendObject();
    }
    if (objects.size() > 0) {
        pError() << "The backend objects are not deleted as was requested.";
        qDeleteAll(objects);
    }
    delete m_backendObject;
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    delete m_platformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

void FactoryPrivate::objectDescriptionChanged(ObjectDescriptionType type)
{
#ifdef PHONON_METHODTEST
    Q_UNUSED(type);
#else
    pDebug() << Q_FUNC_INFO << type;
    switch (type) {
    case AudioOutputDeviceType:
        emit availableAudioOutputDevicesChanged();
        break;
    case AudioCaptureDeviceType:
        emit availableAudioCaptureDevicesChanged();
        break;
    case VideoCaptureDeviceType:
        emit availableVideoCaptureDevicesChanged();
        break;
    default:
        break;
    }
    //emit capabilitiesChanged();
#endif // PHONON_METHODTEST
}

Factory::Sender *Factory::sender()
{
    return globalFactory;
}

bool Factory::isMimeTypeAvailable(const QString &mimeType)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f) {
        return f->isMimeTypeAvailable(mimeType);
    }
#else
    Q_UNUSED(mimeType);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    return true; // the MIME type might be supported, let BackendCapabilities find out
}

void Factory::registerFrontendObject(MediaNodePrivate *bp)
{
    globalFactory->mediaNodePrivateList.prepend(bp); // inserted last => deleted first
}

void Factory::deregisterFrontendObject(MediaNodePrivate *bp)
{
    // The Factory can already be cleaned up while there are other frontend objects still alive.
    // When those are deleted they'll call deregisterFrontendObject through ~BasePrivate
    if (!globalFactory.isDestroyed()) {
        globalFactory->mediaNodePrivateList.removeAll(bp);
    }
}

#ifndef PHONON_NO_DBUS
void FactoryPrivate::phononBackendChanged()
{
    QMessageBox::information(qApp->activeWindow(),
                             tr("Restart Application"),
                             tr("You changed the backend of the Phonon multimedia system.\n\n"
                                "To apply this change you will need to"
                                " restart '%1'.").arg(qAppName()));
    emit backendChanged();
}
#endif //PHONON_NO_DBUS

//X void Factory::freeSoundcardDevices()
//X {
//X     if (globalFactory->backend) {
//X         globalFactory->backend->freeSoundcardDevices();
//X     }
//X }

void FactoryPrivate::objectDestroyed(QObject * obj)
{
    //pDebug() << Q_FUNC_INFO << obj;
    objects.removeAll(obj);
}

#define FACTORY_IMPL(classname) \
QObject *Factory::create ## classname(QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent)); \
    } \
    return 0; \
}
#define FACTORY_IMPL_1ARG(classname) \
QObject *Factory::create ## classname(int arg1, QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent, QList<QVariant>() << arg1)); \
    } \
    return 0; \
}

FACTORY_IMPL(MediaObject)
#ifndef QT_NO_PHONON_EFFECT
FACTORY_IMPL_1ARG(Effect)
#endif //QT_NO_PHONON_EFFECT
#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(VolumeFaderEffect)
#endif //QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(AudioOutput)
#ifndef QT_NO_PHONON_VIDEO
FACTORY_IMPL(VideoWidget)
FACTORY_IMPL(VideoGraphicsObject)
#endif //QT_NO_PHONON_VIDEO
FACTORY_IMPL(AudioDataOutput)

#undef FACTORY_IMPL

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
PlatformPlugin *FactoryPrivate::platformPlugin()
{
    if (m_platformPlugin) {
        return m_platformPlugin;
    }
    if (m_noPlatformPlugin) {
        return 0;
    }
#ifndef PHONON_NO_DBUS
    if (!QCoreApplication::instance() || QCoreApplication::applicationName().isEmpty()) {
        pWarning() << "Phonon needs QCoreApplication::applicationName to be set to export audio output names through the DBUS interface";
    }
#endif
    Q_ASSERT(QCoreApplication::instance());
    const QByteArray platform_plugin_env = qgetenv("PHONON_PLATFORMPLUGIN");
    if (!platform_plugin_env.isEmpty()) {
        pDebug() << Q_FUNC_INFO << "platform plugin path:" << platform_plugin_env;
        QPluginLoader pluginLoader(QString::fromLocal8Bit(platform_plugin_env.constData()));
        if (pluginLoader.load()) {
            QObject *plInstance = pluginLoader.instance();
            if (!plInstance) {
                pDebug() << Q_FUNC_INFO << "unable to grab root component object for the platform plugin";
            }

            m_platformPlugin = qobject_cast<PlatformPlugin *>(plInstance);
            if (m_platformPlugin) {
                pDebug() << Q_FUNC_INFO << "platform plugin" << m_platformPlugin->applicationName();
                return m_platformPlugin;
            } else {
                pDebug() << Q_FUNC_INFO << "platform plugin cast fail" << plInstance;
            }
        }
    }
    const QString suffix(QLatin1String("/phonon_platform/"));
    ensureLibraryPathSet();
    QDir dir;
    dir.setNameFilters(
            !qgetenv("KDE_FULL_SESSION").isEmpty() ? QStringList(QLatin1String("kde.*")) :
            (!qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty() ? QStringList(QLatin1String("gnome.*")) :
             QStringList())
            );
    dir.setFilter(QDir::Files);
    const QStringList libPaths = QCoreApplication::libraryPaths();
    forever {
        for (int i = 0; i < libPaths.count(); ++i) {
            const QString libPath = libPaths.at(i) + suffix;
            dir.setPath(libPath);
            if (!dir.exists()) {
                continue;
            }
            const QStringList files = dir.entryList(QDir::Files);
            for (int i = 0; i < files.count(); ++i) {
                pDebug() << "attempting to load" << libPath + files.at(i);
                QPluginLoader pluginLoader(libPath + files.at(i));
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  platform plugin load failed:"
                        << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                QObject *qobj = pluginLoader.instance();
                m_platformPlugin = qobject_cast<PlatformPlugin *>(qobj);
                pDebug() << m_platformPlugin;
                if (m_platformPlugin) {
                    connect(qobj, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
                            SLOT(objectDescriptionChanged(ObjectDescriptionType)));
                    return m_platformPlugin;
                } else {
                    delete qobj;
                    pDebug() << Q_FUNC_INFO << dir.absolutePath() << "exists but the platform plugin was not loadable:" << pluginLoader.errorString();
                    pluginLoader.unload();
                }
            }
        }
        if (dir.nameFilters().isEmpty()) {
            break;
        }
        dir.setNameFilters(QStringList());
    }
    pDebug() << Q_FUNC_INFO << "platform plugin could not be loaded";
    m_noPlatformPlugin = true;
    return 0;
}

PlatformPlugin *Factory::platformPlugin()
{
    return globalFactory->platformPlugin();
}
#endif // QT_NO_PHONON_PLATFORMPLUGIN

QObject *Factory::backend(bool createWhenNull)
{
    if (globalFactory.isDestroyed()) {
        return 0;
    }
    if (createWhenNull && globalFactory->m_backendObject == 0) {
        globalFactory->createBackend();
        // XXX: might create "reentrancy" problems:
        // a method calls this method and is called again because the
        // backendChanged signal is emitted
        if (globalFactory->m_backendObject) {
            emit globalFactory->backendChanged();
        }
    }
    return globalFactory->m_backendObject;
}

#ifndef QT_NO_PROPERTIES
#define GET_STRING_PROPERTY(name) \
QString Factory::name() \
{ \
    if (globalFactory->m_backendObject) { \
        return globalFactory->m_backendObject->property(#name).toString(); \
    } \
    return QString(); \
} \

GET_STRING_PROPERTY(identifier)
GET_STRING_PROPERTY(backendName)
GET_STRING_PROPERTY(backendComment)
GET_STRING_PROPERTY(backendVersion)
GET_STRING_PROPERTY(backendIcon)
GET_STRING_PROPERTY(backendWebsite)
#endif //QT_NO_PROPERTIES
QObject *Factory::registerQObject(QObject *o)
{
    if (o) {
        QObject::connect(o, SIGNAL(destroyed(QObject*)), globalFactory, SLOT(objectDestroyed(QObject*)), Qt::DirectConnection);
        globalFactory->objects.append(o);
    }
    return o;
}

} //namespace Phonon

#include "factory.moc"
#include "moc_factory_p.cpp"

// vim: sw=4 ts=4
