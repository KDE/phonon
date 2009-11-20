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

#ifdef HAVE_PULSEAUDIO
#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>
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

static PulseSupport* s_instance = NULL;
static bool s_pulseActive = false;

static pa_glib_mainloop *s_mainloop = NULL;
static pa_context *s_context = NULL;

static void pulseContextStateCallback(pa_context *context, void *userdata)
{
    PulseUserData *d = reinterpret_cast<PulseUserData *>(userdata);
    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_READY:
            s_pulseActive = true;
            d->initialConnectionLoopExit();
            break;
        case PA_CONTEXT_FAILED:
            d->initialConnectionLoopExit();
            break;
        default:
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
    s_context = pa_context_new(api, "KDE");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(s_context, NULL, static_cast<pa_context_flags_t>(0), 0) >= 0) {
        pa_context_set_state_callback(s_context, &pulseContextStateCallback, userData);
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
