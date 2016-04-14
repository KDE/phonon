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
#include "qsettingsgroup_p.h"
#include "phononnamespace_p.h"
#include "phononnamespace.h"
#include "pulsesupport.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

namespace Phonon
{

GlobalConfigPrivate::GlobalConfigPrivate() : config(QLatin1String("kde.org"), QLatin1String("libphonon"))
{
}

GlobalConfig::GlobalConfig()
    : k_ptr(new GlobalConfigPrivate)
{
}

GlobalConfig::~GlobalConfig()
{
    delete k_ptr;
}

enum WhatToFilter {
    FilterAdvancedDevices = 1,
    FilterHardwareDevices = 2,
    FilterUnavailableDevices = 4
};

static void filter(ObjectDescriptionType type, BackendInterface *backendIface, QList<int> *list, int whatToFilter)
{
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

#ifndef QT_NO_PHONON_SETTINGSGROUP
static QList<int> sortDevicesByCategoryPriority(const GlobalConfig *config, const QSettingsGroup *backendConfig, ObjectDescriptionType type, Category category, QList<int> &defaultList)
{
    Q_ASSERT(config); Q_UNUSED(config);
    Q_ASSERT(backendConfig);
    Q_ASSERT(type == AudioOutputDeviceType);

    if (defaultList.size() <= 1) {
        // nothing to sort
        return defaultList;
    } else {
        // make entries unique
        QSet<int> seen;
        QMutableListIterator<int> it(defaultList);
        while (it.hasNext()) {
            if (seen.contains(it.next())) {
                it.remove();
            } else {
                seen.insert(it.value());
            }
        }
    }

    QList<int> deviceList;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
        deviceList = pulse->objectIndexesByCategory(type, category);
    } else {
        QString categoryKey = QLatin1String("Category_") + QString::number(static_cast<int>(category));
        if (!backendConfig->hasKey(categoryKey)) {
            // no list in config for the given category
            categoryKey = QLatin1String("Category_") + QString::number(static_cast<int>(NoCategory));
            if (!backendConfig->hasKey(categoryKey)) {
                // no list in config for NoCategory
                return defaultList;
            }
        }

        //Now the list from d->config
        deviceList = backendConfig->value(categoryKey, QList<int>());
    }

    //if there are devices in d->config that the backend doesn't report, remove them from the list
    QMutableListIterator<int> i(deviceList);
    while (i.hasNext()) {
        if (0 == defaultList.removeAll(i.next())) {
            i.remove();
        }
    }

    //if the backend reports more devices that are not in d->config append them to the list
    deviceList += defaultList;

    return deviceList;
}

static QList<int> sortDevicesByCategoryPriority(const GlobalConfig *config, const QSettingsGroup *backendConfig, ObjectDescriptionType type, CaptureCategory category, QList<int> &defaultList)
{
    Q_ASSERT(config); Q_UNUSED(config);
    Q_ASSERT(backendConfig);
    Q_ASSERT(type == AudioCaptureDeviceType || type == VideoCaptureDeviceType);

    if (defaultList.size() <= 1) {
        // nothing to sort
        return defaultList;
    } else {
        // make entries unique
        QSet<int> seen;
        QMutableListIterator<int> it(defaultList);
        while (it.hasNext()) {
            if (seen.contains(it.next())) {
                it.remove();
            } else {
                seen.insert(it.value());
            }
        }
    }

    QList<int> deviceList;
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
        deviceList = pulse->objectIndexesByCategory(type, category);
    } else {
        QString categoryKey = QLatin1String("Category_") + QString::number(static_cast<int>(category));
        if (!backendConfig->hasKey(categoryKey)) {
            // no list in config for the given category
            categoryKey = QLatin1String("Category_") + QString::number(static_cast<int>(NoCategory));
            if (!backendConfig->hasKey(categoryKey)) {
                // no list in config for NoCategory
                return defaultList;
            }
        }

        //Now the list from d->config
        deviceList = backendConfig->value(categoryKey, QList<int>());
    }

    //if there are devices in d->config that the backend doesn't report, remove them from the list
    QMutableListIterator<int> i(deviceList);
    while (i.hasNext()) {
        if (0 == defaultList.removeAll(i.next())) {
            i.remove();
        }
    }

    //if the backend reports more devices that are not in d->config append them to the list
    deviceList += defaultList;

    return deviceList;
}

bool GlobalConfig::hideAdvancedDevices() const
{
    P_D(const GlobalConfig);
    //The devices need to be stored independently for every backend
    const QSettingsGroup generalGroup(&d->config, QLatin1String("General"));
    return generalGroup.value(QLatin1String("HideAdvancedDevices"), true);
}

void GlobalConfig::setHideAdvancedDevices(bool hide)
{
    P_D(GlobalConfig);
    QSettingsGroup generalGroup(&d->config, QLatin1String("General"));
    generalGroup.setValue(QLatin1String("HideAdvancedDevices"), hide);
}
#endif // QT_NO_PHONON_SETTINGSGROUP

static bool isHiddenAudioOutputDevice(const GlobalConfig *config, int i)
{
    Q_ASSERT(config);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    if (!config->hideAdvancedDevices())
        return false;
#endif // QT_NO_PHONON_SETTINGSGROUP

    AudioOutputDevice ad = AudioOutputDevice::fromIndex(i);
    const QVariant var = ad.property("isAdvanced");
    return (var.isValid() && var.toBool());
}

#ifndef PHONON_NO_AUDIOCAPTURE
static bool isHiddenAudioCaptureDevice(const GlobalConfig *config, int i)
{
    Q_ASSERT(config);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    if (!config->hideAdvancedDevices())
        return false;
#endif // QT_NO_PHONON_SETTINGSGROUP

    AudioCaptureDevice ad = AudioCaptureDevice::fromIndex(i);
    const QVariant var = ad.property("isAdvanced");
    return (var.isValid() && var.toBool());
}
#endif

#ifndef PHONON_NO_VIDEOCAPTURE
static bool isHiddenVideoCaptureDevice(const GlobalConfig *config, int i)
{
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
    Q_ASSERT(config);
    Q_ASSERT(type == AudioOutputDeviceType);
    Q_UNUSED(type);

    /*QString sb;
    sb = QString("(Size %1)").arg(currentList.size());
    foreach (int i, currentList)
    sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex Current: %s\n", sb.toUtf8().constData());
    sb = QString("(Size %1)").arg(newOrder.size());
    foreach (int i, newOrder)
    sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex Before : %s\n", sb.toUtf8().constData());*/

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

    /*sb = QString("(Size %1)").arg(newList.size());
    foreach (int i, newList)
    sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex After  : %s\n", sb.toUtf8().constData());*/
    return newList;
}

static QList<int> reindexList(const GlobalConfig *config, ObjectDescriptionType type, CaptureCategory category, QList<int>newOrder)
{
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
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
        pulse->setOutputDevicePriorityForCategory(category, order);
        return;
    }

#ifndef QT_NO_PHONON_SETTINGSGROUP
    P_D(GlobalConfig);
    QSettingsGroup backendConfig(&d->config, QLatin1String("AudioOutputDevice")); // + Factory::identifier());

    order = reindexList(this, AudioOutputDeviceType, category, order);

    const QList<int> noCategoryOrder = audioOutputDeviceListFor(NoCategory, ShowUnavailableDevices|ShowAdvancedDevices);
    if (category != NoCategory && order == noCategoryOrder) {
        backendConfig.removeEntry(QLatin1String("Category_") + QString::number(category));
    } else {
        backendConfig.setValue(QLatin1String("Category_") + QString::number(category), order);
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
}

QList<int> GlobalConfig::audioOutputDeviceListFor(Category category, int override) const
{
    P_D(const GlobalConfig);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    const bool hide = ((override & AdvancedDevicesFromSettings)
            ? hideAdvancedDevices()
            : static_cast<bool>(override & HideAdvancedDevices));
#else
    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));
#endif

    QList<int> defaultList;

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
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

#ifndef QT_NO_PHONON_SETTINGSGROUP
    const QSettingsGroup backendConfig(&d->config, QLatin1String("AudioOutputDevice")); // + Factory::identifier());
    return sortDevicesByCategoryPriority(this, &backendConfig, AudioOutputDeviceType, category, defaultList);
#else //QT_NO_PHONON_SETTINGSGROUP
    return defaultList;
#endif //QT_NO_PHONON_SETTINGSGROUP
}


int GlobalConfig::audioOutputDeviceFor(Category category, int override) const
{
#ifndef QT_NO_PHONON_SETTINGSGROUP
    QList<int> ret = audioOutputDeviceListFor(category, override);
    if (!ret.isEmpty())
        return ret.first();
#endif //QT_NO_PHONON_SETTINGSGROUP
    return -1;
}

QHash<QByteArray, QVariant> GlobalConfig::audioOutputDeviceProperties(int index) const
{
    return deviceProperties(AudioOutputDeviceType, index);
}


#ifndef PHONON_NO_AUDIOCAPTURE
void GlobalConfig::setAudioCaptureDeviceListFor(CaptureCategory category, QList<int> order)
{

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
        pulse->setCaptureDevicePriorityForCategory(category, order);
        return;
    }

#ifndef QT_NO_PHONON_SETTINGSGROUP
    P_D(GlobalConfig);
    QSettingsGroup backendConfig(&d->config, QLatin1String("AudioCaptureDevice")); // + Factory::identifier());

    order = reindexList(this, AudioCaptureDeviceType, category, order);

    const QList<int> noCategoryOrder = audioCaptureDeviceListFor(NoCaptureCategory, ShowUnavailableDevices|ShowAdvancedDevices);
    if (category != NoCaptureCategory && order == noCategoryOrder) {
        backendConfig.removeEntry(QLatin1String("Category_") + QString::number(category));
    } else {
        backendConfig.setValue(QLatin1String("Category_") + QString::number(category), order);
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
}

QList<int> GlobalConfig::audioCaptureDeviceListFor(CaptureCategory category, int override) const
{
    P_D(const GlobalConfig);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    const bool hide = ((override & AdvancedDevicesFromSettings)
        ? hideAdvancedDevices()
        : static_cast<bool>(override & HideAdvancedDevices));
#else
    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));
#endif

    QList<int> defaultList;

    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
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

#ifndef QT_NO_PHONON_SETTINGSGROUP
    const QSettingsGroup backendConfig(&d->config, QLatin1String("AudioCaptureDevice")); // + Factory::identifier());
    return sortDevicesByCategoryPriority(this, &backendConfig, AudioCaptureDeviceType, category, defaultList);
#else //QT_NO_PHONON_SETTINGSGROUP
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    return defaultList;
#else //QT_NO_PHONON_PLATFORMPLUGIN
    return QList<int>();
#endif //QT_NO_PHONON_PLATFORMPLUGIN
#endif //QT_NO_PHONON_SETTINGSGROUP
}

int GlobalConfig::audioCaptureDeviceFor(CaptureCategory category, int override) const
{
    QList<int> ret = audioCaptureDeviceListFor(category, override);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

QHash<QByteArray, QVariant> GlobalConfig::audioCaptureDeviceProperties(int index) const
{
    return deviceProperties(AudioCaptureDeviceType, index);
}

void GlobalConfig::setAudioCaptureDeviceListFor(Category category, QList<int> order)
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    setAudioCaptureDeviceListFor(cat, order);
}

QList<int> GlobalConfig::audioCaptureDeviceListFor(Category category, int override) const
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    return audioCaptureDeviceListFor(cat, override);
}

int GlobalConfig::audioCaptureDeviceFor(Category category, int override) const
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    return audioCaptureDeviceFor(cat, override);
}

#endif //PHONON_NO_AUDIOCAPTURE


#ifndef PHONON_NO_VIDEOCAPTURE
void GlobalConfig::setVideoCaptureDeviceListFor(CaptureCategory category, QList<int> order)
{
#ifndef QT_NO_PHONON_SETTINGSGROUP
    P_D(GlobalConfig);
    QSettingsGroup backendConfig(&d->config, QLatin1String("VideoCaptureDevice")); // + Factory::identifier());

    order = reindexList(this, VideoCaptureDeviceType, category, order);

    const QList<int> noCategoryOrder = videoCaptureDeviceListFor(NoCaptureCategory, ShowUnavailableDevices|ShowAdvancedDevices);
    if (category != NoCaptureCategory && order == noCategoryOrder) {
        backendConfig.removeEntry(QLatin1String("Category_") + QString::number(category));
    } else {
        backendConfig.setValue(QLatin1String("Category_") + QString::number(category), order);
    }
#endif //QT_NO_PHONON_SETTINGSGROUP
}

QList<int> GlobalConfig::videoCaptureDeviceListFor(CaptureCategory category, int override) const
{
    P_D(const GlobalConfig);

#ifndef QT_NO_PHONON_SETTINGSGROUP
    const bool hide = ((override & AdvancedDevicesFromSettings)
        ? hideAdvancedDevices()
        : static_cast<bool>(override & HideAdvancedDevices));
#else
    const bool hide = !((override & AdvancedDevicesFromSettings) && static_cast<bool>(override & HideAdvancedDevices));
#endif

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

#ifndef QT_NO_PHONON_SETTINGSGROUP
    if (hideAdvancedDevices() || (override & HideUnavailableDevices)) {
        filter(VideoCaptureDeviceType, backendIface, &defaultList,
            (hideAdvancedDevices() ? FilterAdvancedDevices : 0) |
            ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
            );
    }

    //The devices need to be stored independently for every backend
    const QSettingsGroup backendConfig(&d->config, QLatin1String("VideoCaptureDevice")); // + Factory::identifier());
    return sortDevicesByCategoryPriority(this, &backendConfig, VideoCaptureDeviceType, category, defaultList);
#else // QT_NO_PHONON_SETTINGSGROUP
    return defaultList;
#endif // QT_NO_PHONON_SETTINGSGROUP
}

int GlobalConfig::videoCaptureDeviceFor(CaptureCategory category, int override) const
{
    QList<int> ret = videoCaptureDeviceListFor(category, override);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

QHash<QByteArray, QVariant> GlobalConfig::videoCaptureDeviceProperties(int index) const
{
    return deviceProperties(VideoCaptureDeviceType, index);
}

void GlobalConfig::setVideoCaptureDeviceListFor(Category category, QList<int> order)
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    setVideoCaptureDeviceListFor(cat, order);
}

QList<int> GlobalConfig::videoCaptureDeviceListFor(Category category, int override) const
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    return videoCaptureDeviceListFor(cat, override);
}

int GlobalConfig::videoCaptureDeviceFor(Category category, int override) const
{
    CaptureCategory cat = categoryToCaptureCategory(category);
    return videoCaptureDeviceFor(cat, override);
}

#endif // PHONON_NO_VIDEOCAPTURE

QHash<QByteArray, QVariant> GlobalConfig::deviceProperties(ObjectDescriptionType deviceType, int index) const
{
    QList<int> indices;
    QHash<QByteArray, QVariant> props;

    // Try a pulseaudio device
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isUsed()) {
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
