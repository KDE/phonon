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

#ifndef PHONON_GLOBALCONFIG_H
#define PHONON_GLOBALCONFIG_H

#include "phonon_export.h"
#include "phononnamespace.h"
#include "phonondefs.h"
#include "objectdescription.h"


namespace Phonon
{
    class GlobalConfigPrivate;

    class PHONON_EXPORT GlobalConfig
    {
        P_DECLARE_PRIVATE(GlobalConfig)
    public:
        GlobalConfig();
        virtual ~GlobalConfig();

        enum DevicesToHideFlag {
            ShowUnavailableDevices = 0,
            ShowAdvancedDevices = 0,
            HideAdvancedDevices = 1,
            AdvancedDevicesFromSettings = 2,
            HideUnavailableDevices = 4
        };
        bool hideAdvancedDevices() const;
        void setHideAdvancedDevices(bool hide = true);

        QHash<QByteArray, QVariant> deviceProperties(ObjectDescriptionType deviceType, int index) const;

        void setAudioOutputDeviceListFor(Category category, QList<int> order);
        QList<int> audioOutputDeviceListFor(Category category, int override = AdvancedDevicesFromSettings) const;
        int audioOutputDeviceFor(Category category, int override = AdvancedDevicesFromSettings) const;
        QHash<QByteArray, QVariant> audioOutputDeviceProperties(int index) const;

#ifndef PHONON_NO_VIDEOCAPTURE
        void setVideoCaptureDeviceListFor(CaptureCategory category, QList<int> order);
        QList<int> videoCaptureDeviceListFor(CaptureCategory category, int override = AdvancedDevicesFromSettings) const;
        int videoCaptureDeviceFor(CaptureCategory category, int override = AdvancedDevicesFromSettings) const;
        QHash<QByteArray, QVariant> videoCaptureDeviceProperties(int index) const;

        PHONON_DEPRECATED void setVideoCaptureDeviceListFor(Category category, QList<int> order);
        PHONON_DEPRECATED QList<int> videoCaptureDeviceListFor(Category category, int override = AdvancedDevicesFromSettings) const;
        PHONON_DEPRECATED int videoCaptureDeviceFor(Category category, int override = AdvancedDevicesFromSettings) const;
#endif //PHONON_NO_VIDEOCAPTURE

#ifndef PHONON_NO_AUDIOCAPTURE
        void setAudioCaptureDeviceListFor(CaptureCategory category, QList<int> order);
        QList<int> audioCaptureDeviceListFor(CaptureCategory category, int override = AdvancedDevicesFromSettings) const;
        int audioCaptureDeviceFor(CaptureCategory category, int override = AdvancedDevicesFromSettings) const;
        QHash<QByteArray, QVariant> audioCaptureDeviceProperties(int index) const;

        PHONON_DEPRECATED void setAudioCaptureDeviceListFor(Category category, QList<int> order);
        PHONON_DEPRECATED QList<int> audioCaptureDeviceListFor(Category category, int override = AdvancedDevicesFromSettings) const;
        PHONON_DEPRECATED int audioCaptureDeviceFor(Category category, int override = AdvancedDevicesFromSettings) const;
#endif //PHONON_NO_AUDIOCAPTURE

    protected:
        GlobalConfigPrivate *const k_ptr;
    };
} // namespace Phonon


#endif // PHONON_GLOBALCONFIG_H
