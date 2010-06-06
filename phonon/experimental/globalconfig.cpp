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
#include "../globalconfig_p.h"

#include "../factory_p.h"
#include "../phonondefs_p.h"
#include "../backendinterface.h"
#include "../qsettingsgroup_p.h"
#include "../platformplugin.h"
#include "pulsesupport.h"

#include <QtCore/QList>
#include <QtCore/QVariant>

namespace Phonon
{
namespace Experimental
{

enum WhatToFilter {
    FilterAdvancedDevices = 1,
    FilterUnavailableDevices = 4
};

static void filter(BackendInterface *backendIface, QList<int> *list, int whatToFilter)
{
    QMutableListIterator<int> it(*list);
    while (it.hasNext()) {
        const QHash<QByteArray, QVariant> properties = backendIface->objectDescriptionProperties(
                static_cast<Phonon::ObjectDescriptionType>(Phonon::Experimental::VideoCaptureDeviceType), it.next());
        QVariant var;
        if (whatToFilter & FilterAdvancedDevices) {
            var = properties.value("isAdvanced");
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

QList<int> GlobalConfig::videoCaptureDeviceListFor(Phonon::Category category, int override) const
{
    K_D(const GlobalConfig);

    //The devices need to be stored independently for every backend
    const QSettingsGroup backendConfig(&d->config, QLatin1String("VideoCaptureDevice")); // + Factory::identifier());
    const QSettingsGroup generalGroup(&d->config, QLatin1String("General"));
    const bool hideAdvancedDevices = ((override & AdvancedDevicesFromSettings)
            ? generalGroup.value(QLatin1String("HideAdvancedDevices"), true)
            : static_cast<bool>(override & HideAdvancedDevices));

    //First we lookup the available devices directly from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Phonon::Factory::backend());
    if (!backendIface) {
        return QList<int>();
    }

    // this list already is in default order (as defined by the backend)
    QList<int> defaultList = backendIface->objectDescriptionIndexes(static_cast<Phonon::ObjectDescriptionType>(Phonon::Experimental::VideoCaptureDeviceType));
    if (hideAdvancedDevices || (override & HideUnavailableDevices)) {
        filter(backendIface, &defaultList,
                (hideAdvancedDevices ? FilterAdvancedDevices : 0) |
                ((override & HideUnavailableDevices) ? FilterUnavailableDevices : 0)
                );
    }

    QString categoryKey = QLatin1String("Category") + QString::number(static_cast<int>(category));
    if (!backendConfig.hasKey(categoryKey)) {
        // no list in config for the given category
        QString categoryKey = QLatin1String("Category") + QString::number(static_cast<int>(Phonon::NoCategory));
        if (!backendConfig.hasKey(categoryKey)) {
            // no list in config for NoCategory
            return defaultList;
        }
    }

    //Now the list from d->config
    QList<int> deviceList = backendConfig.value(categoryKey, QList<int>());

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

int GlobalConfig::videoCaptureDeviceFor(Phonon::Category category, int override) const
{
    QList<int> ret = videoCaptureDeviceListFor(category, override);
    if (ret.isEmpty())
        return -1;
    return ret.first();
}

QHash<QByteArray, QVariant> GlobalConfig::audioOutputDeviceProperties(int index) const
{
    return deviceProperties(static_cast<Phonon::Experimental::ObjectDescriptionType>(AudioOutputDeviceType), index);
}

QHash<QByteArray, QVariant> GlobalConfig::audioCaptureDeviceProperties(int index) const
{
    return deviceProperties(static_cast<Phonon::Experimental::ObjectDescriptionType>(AudioCaptureDeviceType), index);
}

QHash<QByteArray, QVariant> GlobalConfig::videoCaptureDeviceProperties(int index) const
{
    return deviceProperties(VideoCaptureDeviceType, index);
}

QHash<QByteArray, QVariant> GlobalConfig::deviceProperties(Phonon::Experimental::ObjectDescriptionType deviceType, int index) const
{
    #ifndef QT_NO_PHONON_SETTINGSGROUP

    // Try a pulseaudio device
    PulseSupport *pulse = PulseSupport::getInstance();
    if (pulse->isActive())
        return pulse->objectDescriptionProperties(static_cast<Phonon::ObjectDescriptionType>(deviceType), index);

    #ifndef QT_NO_PHONON_PLATFORMPLUGIN
    // Try a device from the platform
    if (PlatformPlugin *platformPlugin = Factory::platformPlugin())
        return platformPlugin->objectDescriptionProperties(static_cast<Phonon::ObjectDescriptionType>(deviceType), index);
    #endif //QT_NO_PHONON_PLATFORMPLUGIN

    // Try a device from the backend
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    if (backendIface)
        return backendIface->objectDescriptionProperties(static_cast<Phonon::ObjectDescriptionType>(deviceType), index);

    #endif // QT_NO_PHONON_SETTINGSGROUP

    return QHash<QByteArray, QVariant>();
}

} // namespace Experimental
} // namespace Phonon
