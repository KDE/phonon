/*  This file is part of the KDE project
    Copyright (C) 2009 Colin Guthrie <cguthrie@mandriva.org>

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

#include <QtCore/QtGlobal>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>

#ifdef HAVE_PULSEAUDIO
#include <glib.h>
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-device-manager.h>
#endif // HAVE_PULSEAUDIO

#include "pulsesupport_p.h"
#include "phononnamespace.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

#ifdef HAVE_PULSEAUDIO
/***
* Prints a conditional debug message based on the current debug level
* If obj is provided, classname and objectname will be printed as well
*
* see debugLevel()
*/

static int debugLevel() {
    return 2;
}

static void logMessage(const QString &message, int priority = 2, QObject *obj=0);
static void logMessage(const QString &message, int priority, QObject *obj)
{
    if (debugLevel() > 0) {
        QString output;
        if (obj) {
            // Strip away namespace from className
            QString className(obj->metaObject()->className());
            int nameLength = className.length() - className.lastIndexOf(':') - 1;
            className = className.right(nameLength);
            output.sprintf("%s %s (%s %p)", message.toLatin1().constData(), 
                           obj->objectName().toLatin1().constData(), 
                           className.toLatin1().constData(), obj);
        }
        else {
            output = message;
        }
        if (priority <= debugLevel()) {
            qDebug() << QString("PulseSupport(%1): %2").arg(priority).arg(output);
        }
    }
}


class AudioDevice
{
    public:
        inline
        AudioDevice(QString name, unsigned int priority, QString desc, QString icon)
        : pulseName(name)
        , pulsePriority(priority)
        , description(desc)
        , icon(icon)
        , isAdvanced(false)
        {
            /*logMessage(QString("Adding '%1' (priority: %2)").arg(desc).arg(priority));*/
        }

        QString pulseName;
        unsigned int pulsePriority;
        QString description;
        QString icon;
        bool isAdvanced;
};
bool operator<(const AudioDevice &a, const AudioDevice &b)
{
    return a.pulsePriority < b.pulsePriority;
}

class PulseUserData
{
    public:
        inline 
        PulseUserData()
        {
        }

        QMap<Phonon::Category, QList<AudioDevice> > outputDevices;
        QMap<Phonon::Category, QList<AudioDevice> > captureDevices;
};

static QMap<QString, Phonon::Category> s_role_category_map;

static PulseSupport* s_instance = NULL;
static bool s_pulseActive = false;

static pa_glib_mainloop *s_mainloop = NULL;
static pa_context *s_context = NULL;
static QEventLoop *s_connection_eventloop = NULL;

QMap<Phonon::Category, QList<AudioDevice> > s_output_devices;
QMap<Phonon::Category, QList<AudioDevice> > s_capture_devices;


static void ext_device_manager_subscribe_cb(pa_context *, void *);
static void ext_device_manager_read_cb(pa_context *c, const pa_ext_device_manager_info *info, int eol, void *userdata) {
    Q_ASSERT(c);
    Q_ASSERT(userdata);

    // If this is our first iteration, set things up properly
    if (s_connection_eventloop) {
        s_connection_eventloop->exit(0);
        s_connection_eventloop = NULL;
        s_pulseActive = true;

        pa_operation *o;
        pa_ext_device_manager_set_subscribe_cb(c, ext_device_manager_subscribe_cb, NULL);
        if ((o = pa_ext_device_manager_subscribe(c, 1, NULL, NULL)))
            pa_operation_unref(o);
    }

    if (eol < 0) {
        logMessage(QString("Failed to initialize device manager extension: %1").arg(pa_strerror(pa_context_errno(c))));
        s_pulseActive = false;
        return;
    }

    PulseUserData *u = reinterpret_cast<PulseUserData*>(userdata);
    if (eol) {
        // We're done reading the data, so order it by priority and copy it into the
        // static variables where it can then be accessed by those classes that need it.

        QMap<Phonon::Category, QList<AudioDevice> >::iterator it;
        for (it = u->outputDevices.begin(); it != u->outputDevices.end(); ++it) {
            QList<AudioDevice> &list = it.value();
            qSort(list);
        }
        s_output_devices = u->outputDevices;
        for (it = u->captureDevices.begin(); it != u->captureDevices.end(); ++it) {
            QList<AudioDevice> &list = it.value();
            qSort(list);
        }
        s_capture_devices = u->captureDevices;

        // Also free the user data as we will not be called again.
        delete u;

        // Some debug
        logMessage(QString("Output Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_output_devices.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                QList<AudioDevice>::iterator it;
                int j = 0;
                for (it = s_output_devices[cat].begin(); it != s_output_devices[cat].end(); ++it) {
                    logMessage(QString("    %1. %2").arg(++j).arg((*it).description));
                }
            }
        }
        logMessage(QString("Capture Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_capture_devices.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                QList<AudioDevice>::iterator it;
                int j = 0;
                for (it = s_capture_devices[cat].begin(); it != s_capture_devices[cat].end(); ++it) {
                    logMessage(QString("    %1. %2").arg(++j).arg((*it).description));
                }
            }
        }
    }

    if (!info)
        return;

    Q_ASSERT(info->name);
    Q_ASSERT(info->description);
    Q_ASSERT(info->icon);

    // For each role in the priority, map it to a phonon category and store the order.
    for (uint32_t i = 0; i < info->n_role_priorities; ++i) {
        pa_ext_device_manager_role_priority_info* role_prio = &info->role_priorities[i];
        Q_ASSERT(role_prio->role);

        if (s_role_category_map.contains(role_prio->role)) {
            Phonon::Category cat = s_role_category_map[role_prio->role];
            QList<AudioDevice> *list;
            QString name(info->name);

            if (name.startsWith("sink:")) {
                if (!u->outputDevices.contains(cat))
                    u->outputDevices.insert(cat, QList<AudioDevice>());
                list = &u->outputDevices[cat];
            } else if (name.startsWith("source:")) {
                if (!u->captureDevices.contains(cat))
                    u->captureDevices.insert(cat, QList<AudioDevice>());
                list = &u->captureDevices[cat];
            } else {
                continue;
            }

            list->append(AudioDevice(info->name, role_prio->priority, info->description, info->icon));
        }
    }
}

static void ext_device_manager_subscribe_cb(pa_context *c, void *) {
    Q_ASSERT(c);

    pa_operation *o;
    PulseUserData *u = new PulseUserData; /** @todo Make some object to receive the info... */
    if (!(o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, u))) {
        // We need to deal with failure on first iteration
        if (s_connection_eventloop) {
            s_connection_eventloop->exit(0);
            s_connection_eventloop = NULL;
        }
        logMessage(QString("pa_ext_device_manager_read() failed"));
        return;
    }
    pa_operation_unref(o);
}

static void context_state_callback(pa_context *c, void *)
{
    Q_ASSERT(c);

    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY:
            // Attempt to load things up
            ext_device_manager_subscribe_cb(c, NULL);
            break;

        case PA_CONTEXT_FAILED:
            s_pulseActive = false;
            if (s_connection_eventloop) {
                s_connection_eventloop->exit(0);
                s_connection_eventloop = NULL;
            }
            break;

        case PA_CONTEXT_TERMINATED:
        default:
            s_pulseActive = false;
            /// @todo Deal with reconnection...
            break;
    }
}
#endif // HAVE_PULSEAUDIO


PulseSupport* PulseSupport::getInstance()
{
    if (NULL == s_instance) {
        s_instance = new PulseSupport();
    }
    return s_instance;
}

void PulseSupport::shutdown()
{
    if (NULL != s_instance) {
        delete s_instance;
        s_instance = NULL;
    }
}

PulseSupport::PulseSupport()
{
    s_role_category_map["none"] = Phonon::NoCategory;
    s_role_category_map["video"] = Phonon::VideoCategory;
    s_role_category_map["music"] = Phonon::MusicCategory;
    s_role_category_map["game"] = Phonon::GameCategory;
    s_role_category_map["event"] = Phonon::NotificationCategory;
    s_role_category_map["phone"] = Phonon::CommunicationCategory;
    //s_role_category_map["animation"] = Phonon::NoCategory; // No Mapping
    //s_role_category_map["production"] = Phonon::NoCategory; // No Mapping
    s_role_category_map["a11y"] = Phonon::AccessibilityCategory;

#ifdef HAVE_PULSEAUDIO
    // To allow for easy debugging, give an easy way to disable this pulseaudio check
    QString pulseenv = qgetenv("PHONON_DISABLE_PULSEAUDIO");
    if (pulseenv.toInt())
        return;

    s_mainloop = pa_glib_mainloop_new(NULL);
    Q_ASSERT(s_mainloop);
    pa_mainloop_api *api = pa_glib_mainloop_get_api(s_mainloop);

    // We create a simple event loop to allow the glib loop
    // to iterate until we've connected or not to the server.
    s_connection_eventloop = new QEventLoop;

    // XXX I don't want to show up in the client list. All I want to know is the list of sources
    // and sinks...
    s_context = pa_context_new(api, "libphonon");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(s_context, NULL, static_cast<pa_context_flags_t>(0), 0) >= 0) {
        pa_context_set_state_callback(s_context, &context_state_callback, s_connection_eventloop);
        // Now we block until we connect or otherwise...
        s_connection_eventloop->exec();
    }
#endif
}

PulseSupport::~PulseSupport()
{
#ifdef HAVE_PULSEAUDIO
    if (s_context) {
        pa_context_disconnect(s_context);
        s_context = NULL;
    }

    if (s_mainloop) {
        pa_glib_mainloop_free(s_mainloop);
        s_mainloop = NULL;
    }

    if (s_connection_eventloop) {
        delete s_connection_eventloop;
        s_connection_eventloop = NULL;
    }
#endif
}

bool PulseSupport::isActive()
{
#ifdef HAVE_PULSEAUDIO
    return s_pulseActive;
#else
    return false;
#endif
}

} // namespace Phonon

QT_END_NAMESPACE

// vim: sw=4 ts=4
