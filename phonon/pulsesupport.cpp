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
#include <QtCore/QStringList>

#ifdef HAVE_PULSEAUDIO
#include <glib.h>
#include <pulse/pulseaudio.h>
#include <pulse/xmalloc.h>
#include <pulse/glib-mainloop.h>
#include <pulse/ext-device-manager.h>
#endif // HAVE_PULSEAUDIO

#include "pulsesupport_p.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

static PulseSupport* s_instance = NULL;

#ifdef HAVE_PULSEAUDIO
/***
* Prints a conditional debug message based on the current debug level
* If obj is provided, classname and objectname will be printed as well
*
* see debugLevel()
*/

static int debugLevel() {
    static int level = -1;
    if (level < 1) {
        level = 0;
        QString pulseenv = qgetenv("PHONON_PULSEAUDIO_DEBUG");
        int l = pulseenv.toInt();
        if (l > 0)
            level = (l > 2 ? 2 : l);
    }
    return level;
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
        AudioDevice(QString name, QString desc, QString icon, uint32_t index)
        : pulseName(name), pulseIndex(index)
        {
            properties["name"] = desc;
            properties["description"] = ""; // We don't have descriptions (well we do, but we use them as the name!)
            properties["icon"] = icon;
            properties["available"] = (index != PA_INVALID_INDEX);
            properties["isAdvanced"] = false; // Nothing is advanced!
        }

        // Needed for QMap
        inline AudioDevice() {}

        QString pulseName;
        uint32_t pulseIndex;
        QHash<QByteArray, QVariant> properties;
};
bool operator!=(const AudioDevice &a, const AudioDevice &b)
{
    return !(a.pulseName == b.pulseName && a.properties == b.properties);
}

class PulseUserData
{
    public:
        inline 
        PulseUserData()
        {
        }

        QMap<QString, AudioDevice> newOutputDevices;
        QMap<Phonon::Category, QMap<int, int> > newOutputDevicePriorities; // prio, device

        QMap<QString, AudioDevice> newCaptureDevices;
        QMap<Phonon::Category, QMap<int, int> > newCaptureDevicePriorities; // prio, device
};

static QMap<QString, Phonon::Category> s_roleCategoryMap;

static bool s_pulseActive = false;

static pa_glib_mainloop *s_mainloop = NULL;
static pa_context *s_context = NULL;
static QEventLoop *s_connectionEventloop = NULL;



static int s_deviceIndexCounter = 0;

static QMap<QString, int> s_outputDeviceIndexes;
static QMap<int, AudioDevice> s_outputDevices;
static QMap<Phonon::Category, QMap<int, int> > s_outputDevicePriorities; // prio, device
static QMap<QString, uint32_t> s_outputStreamIndexMap;
static QMap<QString, int> s_outputStreamMoveQueue;

static QMap<QString, int> s_captureDeviceIndexes;
static QMap<int, AudioDevice> s_captureDevices;
static QMap<Phonon::Category, QMap<int, int> > s_captureDevicePriorities; // prio, device
static QMap<QString, uint32_t> s_captureStreamIndexMap;
static QMap<QString, int> s_captureStreamMoveQueue;


static void ext_device_manager_subscribe_cb(pa_context *, void *);
static void ext_device_manager_read_cb(pa_context *c, const pa_ext_device_manager_info *info, int eol, void *userdata) {
    Q_ASSERT(c);
    Q_ASSERT(userdata);

    // If this is our first iteration, set things up properly
    if (s_connectionEventloop) {
        s_connectionEventloop->exit(0);
        s_connectionEventloop = NULL;
        s_pulseActive = true;

        pa_operation *o;
        pa_ext_device_manager_set_subscribe_cb(c, ext_device_manager_subscribe_cb, NULL);
        if ((o = pa_ext_device_manager_subscribe(c, 1, NULL, NULL)))
            pa_operation_unref(o);
    }

    if (eol < 0) {
        logMessage(QString("Failed to initialize device manager extension: %1").arg(pa_strerror(pa_context_errno(c))));
        // OK so we don't have the device manager extension, but we can show a single device and fake it.
        int index;
        s_outputDeviceIndexes.clear();
        s_outputDevices.clear();
        s_outputDevicePriorities.clear();
        index = s_deviceIndexCounter++;
        s_outputDeviceIndexes.insert("sink:default", index);
        s_outputDevices.insert(index, AudioDevice("sink:default", QObject::tr("PulseAudio Sound Server").toUtf8(), "audio-backend-pulseaudio", 0));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            s_outputDevicePriorities[cat].insert(0, index);
        }

        s_captureDeviceIndexes.clear();
        s_captureDevices.clear();
        s_captureDevicePriorities.clear();
        index = s_deviceIndexCounter++;
        s_captureDeviceIndexes.insert("source:default", index);
        s_captureDevices.insert(index, AudioDevice("source:default", QObject::tr("PulseAudio Sound Server").toUtf8(), "audio-backend-pulseaudio", 0));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            s_captureDevicePriorities[cat].insert(0, index);
        }

        return;
    }

    PulseUserData *u = reinterpret_cast<PulseUserData*>(userdata);
    if (eol) {
        // We're done reading the data, so order it by priority and copy it into the
        // static variables where it can then be accessed by those classes that need it.

        QMap<QString, AudioDevice>::iterator newdev_it;

        // Check for new output devices or things changing about known output devices.
        bool output_changed = false;
        for (newdev_it = u->newOutputDevices.begin(); newdev_it != u->newOutputDevices.end(); ++newdev_it) {
            QString name = newdev_it.key();

            // The name + index map is always written when a new device is added.
            Q_ASSERT(s_outputDeviceIndexes.contains(name));

            int index = s_outputDeviceIndexes[name];
            if (!s_outputDevices.contains(index)) {
                // This is a totally new device
                output_changed = true;
                logMessage(QString("Brand New Output Device Found."));
                s_outputDevices.insert(index, *newdev_it);
            } else  if (s_outputDevices[index] != *newdev_it) {
                // We have this device already, but is it different?
                output_changed = true;
                logMessage(QString("Change to Existing Output Device (may be Added/Removed or something else)"));
                s_outputDevices.remove(index);
                s_outputDevices.insert(index, *newdev_it);
            }
        }
        // Go through the output devices we know about and see if any are no longer mentioned in the list.
        QMutableMapIterator<QString, int> output_existing_it(s_outputDeviceIndexes);
        while (output_existing_it.hasNext()) {
            output_existing_it.next();
            if (!u->newOutputDevices.contains(output_existing_it.key())) {
                output_changed = true;
                logMessage(QString("Output Device Completely Removed"));
                s_outputDevices.remove(output_existing_it.value());
                output_existing_it.remove();
            }
        }

        // Check for new capture devices or things changing about known capture devices.
        bool capture_changed = false;
        for (newdev_it = u->newCaptureDevices.begin(); newdev_it != u->newCaptureDevices.end(); ++newdev_it) {
            QString name = newdev_it.key();

            // The name + index map is always written when a new device is added.
            Q_ASSERT(s_captureDeviceIndexes.contains(name));

            int index = s_captureDeviceIndexes[name];
            if (!s_captureDevices.contains(index)) {
                // This is a totally new device
                capture_changed = true;
                logMessage(QString("Brand New Capture Device Found."));
                s_captureDevices.insert(index, *newdev_it);
            } else  if (s_captureDevices[index] != *newdev_it) {
                // We have this device already, but is it different?
                capture_changed = true;
                logMessage(QString("Change to Existing Capture Device (may be Added/Removed or something else)"));
                s_captureDevices.remove(index);
                s_captureDevices.insert(index, *newdev_it);
            }
        }
        // Go through the capture devices we know about and see if any are no longer mentioned in the list.
        QMutableMapIterator<QString, int> capture_existing_it(s_captureDeviceIndexes);
        while (capture_existing_it.hasNext()) {
            capture_existing_it.next();
            if (!u->newCaptureDevices.contains(capture_existing_it.key())) {
                capture_changed = true;
                logMessage(QString("Capture Device Completely Removed"));
                s_captureDevices.remove(capture_existing_it.value());
                capture_existing_it.remove();
            }
        }

        // Just copy accross the new priority lists as we know they are valid
        if (s_outputDevicePriorities != u->newOutputDevicePriorities) {
            output_changed = true;
            s_outputDevicePriorities = u->newOutputDevicePriorities;
        }
        if (s_captureDevicePriorities != u->newCaptureDevicePriorities) {
            capture_changed = true;
            s_captureDevicePriorities = u->newCaptureDevicePriorities;
        }

        if (s_instance) {
            if (output_changed)
                s_instance->emitObjectDescriptionChanged(AudioOutputDeviceType);
            if (capture_changed)
                s_instance->emitObjectDescriptionChanged(AudioCaptureDeviceType);
        }

        // We can free the user data as we will not be called again.
        delete u;

        // Some debug
        logMessage(QString("Output Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_outputDevicePriorities.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                int count = 0;
                foreach (int j, s_outputDevicePriorities[cat]) {
                    QHash<QByteArray, QVariant> &props = s_outputDevices[j].properties;
                    logMessage(QString("    %1. %2 (Available: %3)").arg(++count).arg(props["name"].toString()).arg(props["available"].toBool()));
                }
            }
        }
        logMessage(QString("Capture Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_captureDevicePriorities.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                int count = 0;
                foreach (int j, s_captureDevicePriorities[cat]) {
                    QHash<QByteArray, QVariant> &props = s_captureDevices[j].properties;
                    logMessage(QString("    %1. %2 (Available: %3)").arg(++count).arg(props["name"].toString()).arg(props["available"].toBool()));
                }
            }
        }
    }

    if (!info)
        return;

    Q_ASSERT(info->name);
    Q_ASSERT(info->description);
    Q_ASSERT(info->icon);

    // QString wrapper
    QString name(info->name);
    int index;
    QMap<Phonon::Category, QMap<int, int> > *new_prio_map_cats; // prio, device
    QMap<QString, AudioDevice> *new_devices;

    if (name.startsWith("sink:")) {
        new_devices = &u->newOutputDevices;
        new_prio_map_cats = &u->newOutputDevicePriorities;

        if (s_outputDeviceIndexes.contains(name))
            index = s_outputDeviceIndexes[name];
        else
            index = s_outputDeviceIndexes[name] = s_deviceIndexCounter++;
    } else if (name.startsWith("source:")) {
        new_devices = &u->newCaptureDevices;
        new_prio_map_cats = &u->newCaptureDevicePriorities;

        if (s_captureDeviceIndexes.contains(name))
            index = s_captureDeviceIndexes[name];
        else
            index = s_captureDeviceIndexes[name] = s_deviceIndexCounter++;
    } else {
        // This indicates a bug in pulseaudio.
        return;
    }

    // Add the new device itself.
    new_devices->insert(name, AudioDevice(name, info->description, info->icon, info->index));

    // For each role in the priority, map it to a phonon category and store the order.
    for (uint32_t i = 0; i < info->n_role_priorities; ++i) {
        pa_ext_device_manager_role_priority_info* role_prio = &info->role_priorities[i];
        Q_ASSERT(role_prio->role);

        if (s_roleCategoryMap.contains(role_prio->role)) {
            Phonon::Category cat = s_roleCategoryMap[role_prio->role];

            (*new_prio_map_cats)[cat].insert(role_prio->priority, index);
        }
    }
}

static void set_output_device(QString streamUuid)
{
    // If we only have one device, bail. This will be true if we are not using module-device-manager
    if (s_outputDevices.size() < 2)
        return;

    if (!s_outputStreamMoveQueue.contains(streamUuid))
        return;

    if (!s_outputStreamIndexMap.contains(streamUuid))
        return;

    int device = s_outputStreamMoveQueue[streamUuid];
    if (!s_outputDevices.contains(device))
        return;

    // Remove so we don't process twice.
    s_outputStreamMoveQueue.remove(streamUuid);

    uint32_t pulse_device_index = s_outputDevices[device].pulseIndex;
    uint32_t pulse_stream_index = s_outputStreamIndexMap[streamUuid];

    const QVariant var = s_outputDevices[device].properties["name"];
    logMessage(QString("Moving Pulse Sink Input %1 to '%2' (Pulse Sink %3)").arg(pulse_stream_index).arg(var.toString()).arg(pulse_device_index));

    /// @todo Find a way to move the stream without saving it... We don't want to pollute the stream restore db.
    pa_operation* o;
    if (!(o = pa_context_move_sink_input_by_index(s_context, pulse_stream_index, pulse_device_index, NULL, NULL))) {
        logMessage(QString("pa_context_move_sink_input_by_index() failed"));
        return;
    }
    pa_operation_unref(o);
}

static void set_capture_device(QString streamUuid)
{
    // If we only have one device, bail. This will be true if we are not using module-device-manager
    if (s_captureDevices.size() < 2)
        return;

    if (!s_captureStreamMoveQueue.contains(streamUuid))
        return;

    if (!s_captureStreamIndexMap.contains(streamUuid))
        return;

    int device = s_captureStreamMoveQueue[streamUuid];
    if (!s_captureDevices.contains(device))
        return;

    // Remove so we don't process twice.
    s_captureStreamMoveQueue.remove(streamUuid);

    uint32_t pulse_device_index = s_captureDevices[device].pulseIndex;
    uint32_t pulse_stream_index = s_captureStreamIndexMap[streamUuid];

    const QVariant var = s_captureDevices[device].properties["name"];
    logMessage(QString("Moving Pulse Source Output %1 to '%2' (Pulse Sink %3)").arg(pulse_stream_index).arg(var.toString()).arg(pulse_device_index));

    /// @todo Find a way to move the stream without saving it... We don't want to pollute the stream restore db.
    pa_operation* o;
    if (!(o = pa_context_move_source_output_by_index(s_context, pulse_stream_index, pulse_device_index, NULL, NULL))) {
        logMessage(QString("pa_context_move_source_output_by_index() failed"));
        return;
    }
    pa_operation_unref(o);
}

void sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata) {
    Q_UNUSED(userdata);
    Q_ASSERT(c);

    if (eol < 0) {
        if (pa_context_errno(c) == PA_ERR_NOENTITY)
            return;

        logMessage(QString("Sink input callback failure"));
        return;
    }

    if (eol > 0)
        return;

    Q_ASSERT(i);

    // loop through (*i) and extract phonon->streamindex...
    const char *t;
    if ((t = pa_proplist_gets(i->proplist, "phonon.streamid"))) {
        logMessage(QString("Found PulseAudio stream index %1 for Phonon Output Stream %2").arg(i->index).arg(t));
        s_outputStreamIndexMap[QString(t)] = i->index;
        // Process any pending moves...
        set_output_device(QString(t));
    }
}

void source_output_cb(pa_context *c, const pa_source_output_info *i, int eol, void *userdata) {
    Q_UNUSED(userdata);
    Q_ASSERT(c);

    if (eol < 0) {
        if (pa_context_errno(c) == PA_ERR_NOENTITY)
            return;

        logMessage(QString("Source output callback failure"));
        return;
    }

    if (eol > 0)
        return;

    Q_ASSERT(i);

    // loop through (*i) and extract phonon->streamindex...
    const char *t;
    if ((t = pa_proplist_gets(i->proplist, "phonon.streamid"))) {
        logMessage(QString("Found PulseAudio stream index %1 for Phonon Capture Stream %2").arg(i->index).arg(t));
        s_captureStreamIndexMap[QString(t)] = i->index;
        // Process any pending moves...
        set_capture_device(QString(t));
    }
}

static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata) {
    Q_UNUSED(userdata);

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                QString phononid = s_outputStreamIndexMap.key(index);
                if (!phononid.isEmpty()) {
                    logMessage(QString("Removing Phonon Output Stream %1 (it's gone!)").arg(phononid));
                    s_outputStreamIndexMap.remove(phononid);
                    s_outputStreamMoveQueue.remove(phononid);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_input_info(c, index, sink_input_cb, NULL))) {
                    logMessage(QString("pa_context_get_sink_input_info() failed"));
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                QString phononid = s_captureStreamIndexMap.key(index);
                if (!phononid.isEmpty()) {
                    logMessage(QString("Removing Phonon Capture Stream %1 (it's gone!)").arg(phononid));
                    s_captureStreamIndexMap.remove(phononid);
                    s_captureStreamMoveQueue.remove(phononid);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_source_output_info(c, index, source_output_cb, NULL))) {
                    logMessage(QString("pa_context_get_sink_input_info() failed"));
                    return;
                }
                pa_operation_unref(o);
            }
            break;
    }
}


static void ext_device_manager_subscribe_cb(pa_context *c, void *) {
    Q_ASSERT(c);

    pa_operation *o;
    PulseUserData *u = new PulseUserData; /** @todo Make some object to receive the info... */
    if (!(o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, u))) {
        // We need to deal with failure on first iteration
        if (s_connectionEventloop) {
            s_connectionEventloop->exit(0);
            s_connectionEventloop = NULL;
        }
        logMessage(QString("pa_ext_device_manager_read() failed"));
        return;
    }
    pa_operation_unref(o);


    // Register for the stream changes...
    pa_context_set_subscribe_callback(c, subscribe_cb, NULL);

    if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                   (PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                    PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT), NULL, NULL))) {
        logMessage(QString("pa_context_subscribe() failed"));
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
            if (s_connectionEventloop) {
                s_connectionEventloop->exit(0);
                s_connectionEventloop = NULL;
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
 : QObject()
{
#ifdef HAVE_PULSEAUDIO
    // Initialise our map (is there a better way to do this?)
    s_roleCategoryMap["none"] = Phonon::NoCategory;
    s_roleCategoryMap["video"] = Phonon::VideoCategory;
    s_roleCategoryMap["music"] = Phonon::MusicCategory;
    s_roleCategoryMap["game"] = Phonon::GameCategory;
    s_roleCategoryMap["event"] = Phonon::NotificationCategory;
    s_roleCategoryMap["phone"] = Phonon::CommunicationCategory;
    //s_roleCategoryMap["animation"]; // No Mapping
    //s_roleCategoryMap["production"]; // No Mapping
    s_roleCategoryMap["a11y"] = Phonon::AccessibilityCategory;

    // To allow for easy debugging, give an easy way to disable this pulseaudio check
    QString pulseenv = qgetenv("PHONON_PULSEAUDIO_DISABLE");
    if (pulseenv.toInt())
        return;

    s_mainloop = pa_glib_mainloop_new(NULL);
    Q_ASSERT(s_mainloop);
    pa_mainloop_api *api = pa_glib_mainloop_get_api(s_mainloop);

    // We create a simple event loop to allow the glib loop
    // to iterate until we've connected or not to the server.
    s_connectionEventloop = new QEventLoop;

    // XXX I don't want to show up in the client list. All I want to know is the list of sources
    // and sinks...
    s_context = pa_context_new(api, "libphonon");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(s_context, NULL, static_cast<pa_context_flags_t>(0), 0) >= 0) {
        pa_context_set_state_callback(s_context, &context_state_callback, s_connectionEventloop);
        // Now we block until we connect or otherwise...
        s_connectionEventloop->exec();
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

    if (s_connectionEventloop) {
        delete s_connectionEventloop;
        s_connectionEventloop = NULL;
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

void PulseSupport::disable()
{
#ifdef HAVE_PULSEAUDIO
    s_pulseActive = false;
#endif
}

QList<int> PulseSupport::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    QList<int> list;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return list;

#ifdef HAVE_PULSEAUDIO
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType: {
                QMap<QString, int>::iterator it;
                for (it = s_outputDeviceIndexes.begin(); it != s_outputDeviceIndexes.end(); ++it) {
                    list.append(*it);
                }
                break;
            }
            case AudioCaptureDeviceType: {
                QMap<QString, int>::iterator it;
                for (it = s_captureDeviceIndexes.begin(); it != s_captureDeviceIndexes.end(); ++it) {
                    list.append(*it);
                }
                break;
            }
            default:
                break;
        }
    }
#endif

    return list;
}

QHash<QByteArray, QVariant> PulseSupport::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    QHash<QByteArray, QVariant> ret;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return ret;

#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(index);
#else
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType:
                Q_ASSERT(s_outputDevices.contains(index));
                ret = s_outputDevices[index].properties;
                break;

            case AudioCaptureDeviceType:
                Q_ASSERT(s_captureDevices.contains(index));
                ret = s_captureDevices[index].properties;
                break;

            default:
                break;
        }
    }
#endif

    return ret;
}

QList<int> PulseSupport::objectIndexesByCategory(ObjectDescriptionType type, Category category) const
{
    QList<int> ret;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return ret;

#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
#else
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType:
                if (s_outputDevicePriorities.contains(category))
                    ret = s_outputDevicePriorities[category].values();
                break;

            case AudioCaptureDeviceType:
                if (s_captureDevicePriorities.contains(category))
                    ret = s_captureDevicePriorities[category].values();
                break;

            default:
                break;
        }
    }
#endif

    return ret;
}

#ifdef HAVE_PULSEAUDIO
static void setDevicePriority(Category category, QStringList list)
{
    QString role = s_roleCategoryMap.key(category);
    if (role.isEmpty())
        return;

    logMessage(QString("Reindexing %1: %2").arg(role).arg(list.join(", ")));

    char **devices;
    devices = pa_xnew(char *, list.size()+1);
    int i = 0;
    foreach (QString str, list) {
        devices[i++] = pa_xstrdup(str.toUtf8().constData());
    }
    devices[list.size()] = NULL;

    pa_operation *o;
    if (!(o = pa_ext_device_manager_reorder_devices_for_role(s_context, role.toUtf8().constData(), (const char**)devices, NULL, NULL)))
        logMessage(QString("pa_ext_device_manager_reorder_devices_for_role() failed"));
    else
        pa_operation_unref(o);

    for (i = 0; i < list.size(); ++i)
        pa_xfree(devices[i]);
    pa_xfree(devices);
}
#endif

void PulseSupport::setOutputDevicePriorityForCategory(Category category, QList<int> order)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
    Q_UNUSED(order);
#else
    QStringList list;
    QList<int>::iterator it;

    for (it = order.begin(); it != order.end(); ++it) {
        if (s_outputDevices.contains(*it)) {
            list << s_outputDeviceIndexes.key(*it);
        }
    }
    setDevicePriority(category, list);
#endif
}

void PulseSupport::setCaptureDevicePriorityForCategory(Category category, QList<int> order)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
    Q_UNUSED(order);
#else
    QStringList list;
    QList<int>::iterator it;

    for (it = order.begin(); it != order.end(); ++it) {
        if (s_captureDevices.contains(*it)) {
            list << s_captureDeviceIndexes.key(*it);
        }
    }
    setDevicePriority(category, list);
#endif
}

void PulseSupport::setStreamPropList(Category category, QString streamUuid)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
    Q_UNUSED(streamUuid);
#else
    QString role = s_roleCategoryMap.key(category);
    if (role.isEmpty())
        return;

    logMessage(QString("Setting role to %1 for streamindex %2").arg(role).arg(streamUuid));
    setenv("PULSE_PROP_media.role", role.toLatin1().constData(), 1);
    setenv("PULSE_PROP_phonon.streamid", streamUuid.toLatin1().constData(), 1);
#endif
}

void PulseSupport::emitObjectDescriptionChanged(ObjectDescriptionType type)
{
    emit objectDescriptionChanged(type);
}

bool PulseSupport::setOutputDevice(QString streamUuid, int device) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(device);
    return false;
#else
    if (!s_outputDevices.contains(device)) {
        logMessage(QString("Attempting to set Output Device for invalid device id %1.").arg(device));
        return false;
    }
    const QVariant var = s_outputDevices[device].properties["name"];
    logMessage(QString("Attempting to set Output Device to '%1' for Output Stream %2").arg(var.toString()).arg(streamUuid));

    s_outputStreamMoveQueue[streamUuid] = device;
    // Attempt to look up the pulse stream index.
    if (s_outputStreamIndexMap.contains(streamUuid)) {
        logMessage(QString("... Found in map. Moving now"));
        set_output_device(streamUuid);
    } else {
        logMessage(QString("... Not found in map. Saving move for when the stream appears"));
    }
    return true;
#endif
}

bool PulseSupport::setCaptureDevice(QString streamUuid, int device) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(device);
    return false;
#else
    if (!s_captureDevices.contains(device)) {
        logMessage(QString("Attempting to set Capture Device for invalid device id %1.").arg(device));
        return false;
    }
    const QVariant var = s_captureDevices[device].properties["name"];
    logMessage(QString("Attempting to set Capture Device to '%1' for Capture Stream %2").arg(var.toString()).arg(streamUuid));

    s_captureStreamMoveQueue[streamUuid] = device;
    // Attempt to look up the pulse stream index.
    if (s_captureStreamIndexMap.contains(streamUuid)) {
        logMessage(QString("... Found in map. Moving now"));
        set_capture_device(streamUuid);
    } else {
        logMessage(QString("... Not found in map. Saving move for when the stream appears"));
    }
    return true;
#endif
}

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_pulsesupport_p.cpp"

// vim: sw=4 ts=4
