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

#include <stdio.h>
#include <glib.h>
#include <QtCore/QtGlobal>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>

#ifdef HAVE_PULSEAUDIO
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-device-manager.h>
#endif // HAVE_PULSEAUDIO

#include "pulsesupport_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

#ifdef HAVE_PULSEAUDIO
class PulseUserData
{
    public:
        inline 
        PulseUserData(QEventLoop *eventLoop)
            : m_eventLoop(eventLoop)
        {
            Q_ASSERT(eventLoop);
        }

        inline void initialConnectionLoopExit()
        {
            m_eventLoop->exit(0);
            m_eventLoop = NULL;
        }
    private:
        QEventLoop *m_eventLoop;
};

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

static PulseSupport* s_instance = NULL;
static bool s_pulseActive = false;

static pa_glib_mainloop *s_mainloop = NULL;
static pa_context *s_context = NULL;

static void ext_device_manager_read_cb(pa_context *c, const pa_ext_device_manager_info *info, int eol, void *) {
    if (eol < 0) {
        logMessage(QString("Failed to initialize device manager extension: %1").arg(pa_strerror(pa_context_errno(c))));
        return;
    }

    if (!info)
        return;

    /* Do something with a widget when this part is written */
    logMessage(QString("Found: %1 '%2'").arg(info->name).arg(info->description));
}

static void ext_device_manager_subscribe_cb(pa_context *c, void *userdata) {
    pa_operation *o;

    if (!(o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, userdata))) {
        logMessage(QString("pa_ext_device_manager_read() failed"));
        return;
    }

    pa_operation_unref(o);
}

static void context_state_callback(pa_context *c, void *userdata)
{
    PulseUserData *u = reinterpret_cast<PulseUserData *>(userdata);
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY:
            u->initialConnectionLoopExit();

            pa_operation *o;

            /*
            pa_context_set_subscribe_callback(c, subscribe_cb, w);

            if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                           (PA_SUBSCRIPTION_MASK_SINK|
                                            PA_SUBSCRIPTION_MASK_SOURCE|
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                            PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT|
                                            PA_SUBSCRIPTION_MASK_CLIENT|
                                            PA_SUBSCRIPTION_MASK_SERVER|
                                            PA_SUBSCRIPTION_MASK_CARD), NULL, NULL))) {
                show_error(_("pa_context_subscribe() failed"));
                return;
            }
            pa_operation_unref(o);
            */

            if ((o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, u))) {
                pa_operation_unref(o);

                pa_ext_device_manager_set_subscribe_cb(c, ext_device_manager_subscribe_cb, u);

                if ((o = pa_ext_device_manager_subscribe(c, 1, NULL, NULL)))
                    pa_operation_unref(o);

            } else
                logMessage(QString("Failed to initialize stream_restore extension: %1").arg(pa_strerror(pa_context_errno(c))));

            s_pulseActive = true;
            break;

        case PA_CONTEXT_FAILED:
            s_pulseActive = false;
            u->initialConnectionLoopExit();
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
    QEventLoop loop;
    PulseUserData* userData = new PulseUserData(&loop);

    // XXX I don't want to show up in the client list. All I want to know is the list of sources
    // and sinks...
    s_context = pa_context_new(api, "libphonon");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(s_context, NULL, static_cast<pa_context_flags_t>(0), 0) >= 0) {
        pa_context_set_state_callback(s_context, &context_state_callback, userData);
        // Now we block until we connect or otherwise...
        loop.exec();
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
