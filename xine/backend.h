/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef Phonon_XINE_BACKEND_H
#define Phonon_XINE_BACKEND_H

#include <QtCore/QList>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QVariant>

#include <xine.h>
#include <xine/xineutils.h>

#include "xineengine.h"
#include <phonon/objectdescription.h>
#include <phonon/backendinterface.h>
#include <KDE/KSharedConfig>

namespace Phonon
{
namespace Xine
{
    enum MediaStreamType {
        Audio = 1,
        Video = 2,
        StillImage = 4,
        Subtitle = 8,
        AllMedia = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(MediaStreamTypes, MediaStreamType)
} // namespace Xine
} // namespace Phonon
Q_DECLARE_OPERATORS_FOR_FLAGS(Phonon::Xine::MediaStreamTypes)

namespace Phonon
{
class AudioDevice;
namespace Xine
{

class WireCall;
class XineThread;

typedef QHash< int, QHash<QByteArray, QVariant> > ChannelIndexHash;
typedef QHash<ObjectDescriptionType, ChannelIndexHash> ObjectDescriptionHash;

class Backend : public QObject, public Phonon::BackendInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::BackendInterface)
    Q_CLASSINFO("D-Bus Interface", "org.kde.phonon.XineBackendInternal")
    public:
        static Backend *instance();
        Backend(QObject *parent, const QVariantList &args);
        ~Backend();

        QObject *createObject(BackendInterface::Class, QObject *parent, const QList<QVariant> &args);

        QList<int> objectDescriptionIndexes(ObjectDescriptionType) const;
        QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType, int) const;

        bool startConnectionChange(QSet<QObject *>);
        bool connectNodes(QObject *, QObject *);
        bool disconnectNodes(QObject *, QObject *);
        bool endConnectionChange(QSet<QObject *>);

        QStringList availableMimeTypes() const;

    // phonon-xine internal:
        static void addCleanupObject(QObject *o) { instance()->m_cleanupObjects << o; }
        static void removeCleanupObject(QObject *o) { instance()->m_cleanupObjects.removeAll(o); }
        static const QList<QObject *> &cleanupObjects() { return instance()->m_cleanupObjects; }

        static bool deinterlaceDVD();
        static bool deinterlaceVCD();
        static bool deinterlaceFile();
        static int deinterlaceMethod();

        static bool inShutdown() { return instance()->m_inShutdown; }

        static void setObjectDescriptionProperities(ObjectDescriptionType type, int index, const QHash<QByteArray, QVariant> &properities);
        static ObjectDescriptionHash objectDescriptions() { return instance()->m_objectDescriptions; }

        static QList<int> audioOutputIndexes();
        static QHash<QByteArray, QVariant> audioOutputProperties(int audioDevice);

        static QByteArray audioDriverFor(int audioDevice);

        static XineEngine xine() { return instance()->m_xine; }
        static void returnXineEngine(const XineEngine &);
        static XineEngine xineEngineForStream();

    signals:
        void objectDescriptionChanged(ObjectDescriptionType);

    private slots:
        void emitAudioDeviceChange();

    private:
        void checkAudioOutputs();
        void addAudioOutput(int idx, int initialPreference, const QString &n,
                const QString &desc, const QString &ic, const QByteArray &dr,
                bool isAdvanced = false);

        mutable QStringList m_supportedMimeTypes;

        QHash<ObjectDescriptionType, QHash<int, QHash<QByteArray, QVariant> > > m_objectDescriptions;

        struct AudioOutputInfo
        {
            AudioOutputInfo(int idx, int ip, const QString &n, const QString &desc, const QString &ic,
                    const QByteArray &dr)
                : name(n), description(desc), icon(ic), driver(dr),
                index(idx), initialPreference(ip), available(false), isAdvanced(false) {}

            QString name;
            QString description;
            QString icon;
            QByteArray driver;
            int index;
            int initialPreference;
            bool available : 1;
            bool isAdvanced : 1;
            inline bool operator==(const AudioOutputInfo &rhs) const { return name == rhs.name && driver == rhs.driver; }
            inline bool operator<(const AudioOutputInfo &rhs) const { return initialPreference > rhs.initialPreference; }
        };
        QList<AudioOutputInfo> m_audioOutputInfos;
        QList<QObject *> m_cleanupObjects;
        KSharedConfigPtr m_config;
        int m_deinterlaceMethod : 8;
        bool m_deinterlaceDVD : 1;
        bool m_deinterlaceVCD : 1;
        bool m_deinterlaceFile : 1;
        bool m_inShutdown : 1;
        XineThread *m_thread;
        XineEngine m_xine;
        QTimer signalTimer;
        QList<WireCall> m_disconnections;

        QList<XineEngine> m_usedEngines;
        QList<XineEngine> m_freeEngines;

        friend class XineThread;
};
}} // namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_BACKEND_H
