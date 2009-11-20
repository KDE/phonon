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

#include "pulsesupport.h"

#include <QtCore/QtGlobal>

#include <kdebug.h>

#ifdef HAVE_PULSEAUDIO
#include <pulse/pulseaudio.h>
#endif // HAVE_PULSEAUDIO

QT_BEGIN_NAMESPACE

class QVariant;

namespace Phonon
{

#ifdef HAVE_PULSEAUDIO
class PulseUserData
{
    public:
        inline PulseUserData(PulseSupport *p, pa_mainloop_api *api, bool *active)
        : m_pulseSupport(p), m_mainloopApi(api), m_active(active), ready(2)
          {}

        PulseSupport *const m_pulseSupport;

        inline void eol() { if (--ready == 0) { quit(); } }
        inline void quit() { m_mainloopApi->quit(m_mainloopApi, 0); }
        inline void setActive(bool active = true) { *m_active = active; }
    private:
        pa_mainloop_api *const m_mainloopApi;
        bool *m_active;
        int ready;
};

static void pulseContextStateCallback(pa_context *context, void *userdata)
{
    PulseUserData *d = reinterpret_cast<PulseUserData *>(userdata);
    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_READY:
            d->setActive();
            d->quit();
            break;
        case PA_CONTEXT_FAILED:
            d->quit();
            break;
        default:
            break;
    }
}
#endif // HAVE_PULSEAUDIO


PulseSupport::PulseSupport()
{
    m_pulseActive = false;

#ifdef HAVE_PULSEAUDIO
    pa_mainloop *mainloop = pa_mainloop_new();
    Q_ASSERT(mainloop);
    pa_mainloop_api *api = pa_mainloop_get_api(mainloop);
    PulseUserData userData(this, api, &m_pulseActive);
    // XXX I don't want to show up in the client list. All I want to know is the list of sources
    // and sinks...
    pa_context *context = pa_context_new(api, "KDE");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(context, NULL, static_cast<pa_context_flags_t>(0), 0) >= 0) {
        pa_context_set_state_callback(context, &pulseContextStateCallback, &userData);
        pa_mainloop_run(mainloop, NULL);
        pa_context_disconnect(context);
    }
    pa_mainloop_free(mainloop);

#endif
}

#ifdef HAVE_PULSEAUDIO

bool PulseSupport::pulseActive()
{
    return m_pulseActive;
}

#else

bool PulseSupport::pulseActive()
{
    return false;
}

#endif

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_pulsesupport.cpp"

// vim: sw=4 ts=4
