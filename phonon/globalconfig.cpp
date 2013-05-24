/*  This file is part of the KDE project
    Copyright (C) 2006-2008 Matthias Kretz <kretz@kde.org>

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

#include "globalconfig.h"
#include "globalconfig_p.h"

#include "factory_p.h"
#include "phonondefs_p.h"
#include "platformplugin.h"
#include "backendinterface.h"
//#include "qsettingsgroup_p.h"
#include "phononnamespace_p.h"
#include "phononnamespace.h"
#include "pulsesupport.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

namespace Phonon {

GlobalConfigPrivate::GlobalConfigPrivate() : config(QLatin1String("kde.org"), QLatin1String("libphonon"))
{
    qDebug() << Q_FUNC_INFO;
}

GlobalConfig::GlobalConfig()
    : k_ptr(new GlobalConfigPrivate)
{
    qDebug() << Q_FUNC_INFO;
}

GlobalConfig::~GlobalConfig()
{
    qDebug() << Q_FUNC_INFO;
    delete k_ptr;
}

enum WhatToFilter {
    FilterAdvancedDevices = 1,
    FilterHardwareDevices = 2,
    FilterUnavailableDevices = 4
};

static void filter(ObjectDescriptionType type, BackendInterface *backendIface, QList<int> *list, int whatToFilter)
{
    qDebug() << Q_FUNC_INFO;
    QMutableListIterator<int> it(*list);
    while (it.hasNext()) {
        QHash<QByteArray, QVariant> properties;
        if (backendIface)
            properties = backendIface->objectDescriptionProperties(type, it.next());
        else
            properties = PulseSupport::getInstance()->objectDescriptionProperties(type, it.next());
        QVariant var;
        if (whatToFilter & FilterAdvancedDevices) {
            var = properties.value("isAdvanced");
            if (var.isValid() && var.toBool()) {
                it.remove();
                continue;
            }
        }
        if (whatToFilter & FilterHardwareDevices) {
            var = properties.value("isHardwareDevice");
            if (var.isValid() && var.toBool()) {
                it.remove();
                continue;
            }
        }
        if (whatToFilter & FilterUnavailableDevices) {
            var = properties.value("available");
            if (var.isValid() && !var.toBool()) {
                it.remove();
                continue;
            }
        }
    }
}

bool GlobalConfig::hideAdvancedDevices() const
{
    qDebug() << Q_FUNC_INFO;
    P_D(const GlobalConfig);
    return true;
//    //The devices need to be stored independently for every backend
//    const QSettingsGroup generalGroup(&d->config, QLatin1String("General"));
//    return generalGroup.value(QLatin1String("HideAdvancedDevices"), true);
}

void GlobalConfig::setHideAdvancedDevices(bool hide)
{
    qDebug() << Q_FUNC_INFO;
}

static bool isHiddenAudioOutputDevice(const GlobalConfig *config, int i)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(config);

    AudioOutputDevice ad = AudioOutputDevice::fromIndex(i);
    const QVariant var = ad.property("isAdvanced");
    return (var.isValid() && var.toBool());
}

#ifndef PHONON_NO_AUDIOCAPTURE
static bool isHiddenAudioCaptureDevice(const GlobalConfig *config, int i)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(config);

    AudioCaptureDevice ad = AudioCaptureDevice::fromIndex(i);
    const QVariant var = ad.property("isAdvanced");
    return (var.isValid() && var.toBool());
}
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
static bool isHiddenVideoCaptureDevice(const GlobalConfig *config, int i)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(config);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    if (!config->hideAdvancedDevices())
        return false;
#endif // QT_NO_PHONON_SETTINGSGROUP

    VideoCaptureDevice vd = VideoCaptureDevice::fromIndex(i);
    const QVariant var = vd.property("isAdvanced");
    return (var.isValid() && var.toBool());
}
#endif

static QList<int> reindexList(const GlobalConfig *config, ObjectDescriptionType type, Category category, QList<int>newOrder)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(config);
    Q_ASSERT(type == AudioOutputDeviceType);
    Q_UNUSED(type);

    int override = GlobalConfig::ShowUnavailableDevices | GlobalConfig::ShowAdvancedDevices;
    QList<int> currentList = config->audioOutputDeviceListFor(category, override);


    QList<int> newList;

    foreach (int i, newOrder) {
        int found = currentList.indexOf(i);
        if (found < 0) {
            // It's not in the list, so something is odd (e.g. client error). Ignore it.
            continue;
        }

        // Iterate through the list from this point onward. If there are hidden devices
        // immediately following, take them too.
        newList.append(currentList.takeAt(found));

        while (found < currentList.size()) {
            bool hidden = isHiddenAudioOutputDevice(config, currentList.at(found));
            if (!hidden)
                break;

            newList.append(currentList.takeAt(found));
        }
    }

    // If there are any devices left in.. just tack them on the end.
    if (currentList.size() > 0)
        newList += currentList;

    return newList;
}

static QList<int> reindexList(const GlobalConfig *config, ObjectDescriptionType type, CaptureCategory category, QList<int>newOrder)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(config);
    Q_ASSERT(type == AudioCaptureDeviceType || type == VideoCaptureDeviceType);

    QList<int> currentList;
    int override = GlobalConfig::ShowUnavailableDevices | GlobalConfig::ShowAdvancedDevices;

    switch (type) {
#ifndef PHONON_NO_AUDIOCAPTURE
    case AudioCaptureDeviceType:
        currentList = config->audioCaptureDeviceListFor(category, override);
        break;
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
    case VideoCaptureDeviceType:
        currentList = config->videoCaptureDeviceListFor(category, override);
        break;
#endif

    default: ;
    }

    QList<int> newList;

    foreach (int i, newOrder) {
        int found = currentList.indexOf(i);
        if (found < 0) {
            // It's not in the list, so something is odd (e.g. client error). Ignore it.
            continue;
        }

        // Iterate through the list from this point onward. If there are hidden devices
        // immediately following, take them too.
        newList.append(currentList.takeAt(found));

        while (found < currentList.size()) {
            bool hidden = true;

            switch (type) {
#ifndef PHONON_NO_AUDIOCAPTURE
            case AudioCaptureDeviceType:
                hidden = isHiddenAudioCaptureDevice(config, currentList.at(found));
                break;
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
            case VideoCaptureDeviceType:
                hidden = isHiddenVideoCaptureDevice(config, currentList.at(found));
                break;
#endif

            default: ;
            }

            if (!hidden)
                break;

            newList.append(currentList.takeAt(found));
        }
    }

    // If there are any devices left in.. just tack them on the end.
    if (currentList.size() > 0)
        newList += currentList;

    return newList;
}

void GlobalConfig::setAudioOutputDeviceListFor(Category category, QList<int> order)
{
    qDebug() << Q_FUNC_INFO;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        pulse->setOutputDevicePriorityForCategory(category, order);
        return;
    }
}

QList<int> GlobalConfig::audioOutputDeviceListFor(Category category, int override) const
{
    qDebug() << Q_FUNC_INFO;
    P_D(const GlobalConfig);

    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));

    QList<int> defaultList;

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        defaultList = pulse->objectDescriptionIndexes(AudioOutputDeviceType);
        if (hide || (override & HideUnavailableDevices)) {
            filter(AudioOutputDeviceType, NULL, &defaultList,
                    (hide ? FilterAdvancedDevices : 0)
                    | ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
                    );
        }
    } else {
        BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        if (PlatformPlugin *platformPlugin = Factory::platformPlugin()) {
            // the platform plugin lists the audio devices for the platform
            // this list already is in default order (as defined by the platform plugin)
            defaultList = platformPlugin->objectDescriptionIndexes(AudioOutputDeviceType);
            if (hide) {
                QMutableListIterator<int> it(defaultList);
                while (it.hasNext()) {
                    AudioOutputDevice objDesc = AudioOutputDevice::fromIndex(it.next());
                    const QVariant var = objDesc.property("isAdvanced");
                    if (var.isValid() && var.toBool()) {
                        it.remove();
                    }
                }
            }
        }
#endif //QT_NO_PHONON_PLATFORMPLUGIN

        // lookup the available devices directly from the backend
        if (backendIface) {
            // this list already is in default order (as defined by the backend)
            QList<int> list = backendIface->objectDescriptionIndexes(AudioOutputDeviceType);
            if (hide || !defaultList.isEmpty() || (override & HideUnavailableDevices)) {
                filter(AudioOutputDeviceType, backendIface, &list,
                        (hide ? FilterAdvancedDevices : 0)
                        // the platform plugin maybe already provided the hardware devices?
                        | (defaultList.isEmpty() ? 0 : FilterHardwareDevices)
                        | ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
                        );
            }
            defaultList += list;
        }
    }

    return defaultList;
}


int GlobalConfig::audioOutputDeviceFor(Category category, int override) const
{
    qDebug() << Q_FUNC_INFO;
#ifndef QT_NO_PHONON_SETTINGSGROUP
    QList<int> ret = audioOutputDeviceListFor(category, override);
    if (!ret.isEmpty())
        return ret.first();
#endif //QT_NO_PHONON_SETTINGSGROUP
    return -1;
}

QHash<QByteArray, QVariant> GlobalConfig::audioOutputDeviceProperties(int index) const
{
    qDebug() << Q_FUNC_INFO;
    return deviceProperties(AudioOutputDeviceType, index);
}


#ifndef PHONON_NO_AUDIOCAPTURE
void GlobalConfig::setAudioCaptureDeviceListFor(CaptureCategory category, QList<int> order)
{
    qDebug() << Q_FUNC_INFO;
#warning can go away
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        pulse->setCaptureDevicePriorityForCategory(category, order);
        return;
    }
}

QList<int> GlobalConfig::audioCaptureDeviceListFor(CaptureCategory category, int override) const
{
    qDebug() << Q_FUNC_INFO;
    P_D(const GlobalConfig);

    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));

    QList<int> defaultList;

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        defaultList = pulse->objectDescriptionIndexes(AudioCaptureDeviceType);
        if (hide || (override & HideUnavailableDevices)) {
            filter(AudioCaptureDeviceType, NULL, &defaultList,
                    (hide ? FilterAdvancedDevices : 0)
                    | ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
                    );
        }
    } else {
        BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        if (PlatformPlugin *platformPlugin = Factory::platformPlugin()) {
            // the platform plugin lists the audio devices for the platform
            // this list already is in default order (as defined by the platform plugin)
            defaultList += platformPlugin->objectDescriptionIndexes(AudioCaptureDeviceType);
            if (hide) {
                QMutableListIterator<int> it(defaultList);
                while (it.hasNext()) {
                    AudioCaptureDevice objDesc = AudioCaptureDevice::fromIndex(it.next());
                    const QVariant var = objDesc.property("isAdvanced");
                    if (var.isValid() && var.toBool()) {
                        it.remove();
                    }
                }
            }
        }
#endif //QT_NO_PHONON_PLATFORMPLUGIN

        // lookup the available devices directly from the backend
        if (backendIface) {
            // this list already is in default order (as defined by the backend)
            QList<int> list = backendIface->objectDescriptionIndexes(AudioCaptureDeviceType);
            if (hide || !defaultList.isEmpty() || (override & HideUnavailableDevices)) {
                filter(AudioCaptureDeviceType, backendIface, &list,
                        (hide ? FilterAdvancedDevices : 0)
                        // the platform plugin maybe already provided the hardware devices?
                        | (defaultList.isEmpty() ? 0 : FilterHardwareDevices)
                        | ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
                        );
            }
            defaultList += list;
        }
    }

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    return defaultList;
#else //QT_NO_PHONON_PLATFORMPLUGIN
    return QList<int>();
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

int GlobalConfig::audioCaptureDeviceFor(CaptureCategory category, int override) const
{
    qDebug() << Q_FUNC_INFO;
    QList<int> ret = audioCaptureDeviceListFor(category, override);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

QHash<QByteArray, QVariant> GlobalConfig::audioCaptureDeviceProperties(int index) const
{
    qDebug() << Q_FUNC_INFO;
    return deviceProperties(AudioCaptureDeviceType, index);
}

#endif //PHONON_NO_AUDIOCAPTURE


#ifndef PHONON_NO_VIDEOCAPTURE
void GlobalConfig::setVideoCaptureDeviceListFor(CaptureCategory category, QList<int> order)
{
    qDebug() << Q_FUNC_INFO;
#warning PA has no video capture support, albeit all priority crap is defunct since we do not do priorities anymore
}

QList<int> GlobalConfig::videoCaptureDeviceListFor(CaptureCategory category, int override) const
{
    qDebug() << Q_FUNC_INFO;
    P_D(const GlobalConfig);

    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));

    //First we lookup the available devices directly from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    if (!backendIface) {
        return QList<int>();
    }

    // this list already is in default order (as defined by the backend)
    QList<int> defaultList = backendIface->objectDescriptionIndexes(VideoCaptureDeviceType);

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    if (PlatformPlugin *platformPlugin = Factory::platformPlugin()) {
        // the platform plugin lists the video devices for the platform
        // this list already is in default order (as defined by the platform plugin)
        defaultList += platformPlugin->objectDescriptionIndexes(VideoCaptureDeviceType);
        if (hide) {
            QMutableListIterator<int> it(defaultList);
            while (it.hasNext()) {
                VideoCaptureDevice objDesc = VideoCaptureDevice::fromIndex(it.next());
                const QVariant var = objDesc.property("isAdvanced");
                if (var.isValid() && var.toBool()) {
                    it.remove();
                }
            }
        }
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN

    return defaultList;
}

int GlobalConfig::videoCaptureDeviceFor(CaptureCategory category, int override) const
{
    qDebug() << Q_FUNC_INFO;
    QList<int> ret = videoCaptureDeviceListFor(category, override);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

QHash<QByteArray, QVariant> GlobalConfig::videoCaptureDeviceProperties(int index) const
{
    qDebug() << Q_FUNC_INFO;
    return deviceProperties(VideoCaptureDeviceType, index);
}

#endif // PHONON_NO_VIDEOCAPTURE

QHash<QByteArray, QVariant> GlobalConfig::deviceProperties(ObjectDescriptionType deviceType, int index) const
{
    qDebug() << Q_FUNC_INFO;
    QList<int> indices;
    QHash<QByteArray, QVariant> props;

    // Try a pulseaudio device
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive()) {
        // Check the index before passing it to PulseSupport
        indices = pulse->objectDescriptionIndexes(deviceType);
        if (indices.contains(index))
            props = pulse->objectDescriptionProperties(deviceType, index);
    }
    if (!props.isEmpty())
        return props;

    #ifndef QT_NO_PHONON_PLATFORMPLUGIN
    // Try a device from the platform
    if (PlatformPlugin *platformPlugin = Factory::platformPlugin())
        props = platformPlugin->objectDescriptionProperties(deviceType, index);
    if (!props.isEmpty())
        return props;
    #endif //QT_NO_PHONON_PLATFORMPLUGIN

    // Try a device from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    if (backendIface)
        props = backendIface->objectDescriptionProperties(deviceType, index);
    if (!props.isEmpty())
        return props;

    return props;
}

} // namespace Phonon
