/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_EXPERIMENTAL_BACKENDCAPABILITIES_H
#define PHONON_EXPERIMENTAL_BACKENDCAPABILITIES_H

#include "export.h"
#include "objectdescription.h"
#include "../phononnamespace.h"

namespace Phonon
{
namespace Experimental
{
namespace BackendCapabilities
{

    /**
     * Returns the video capture devices the backend supports.
     *
     * \return A list of VideoCaptureDevice objects that give a name and
     * description for every supported video capture device.
     */
    PHONONEXPERIMENTAL_EXPORT QList<VideoCaptureDevice> availableVideoCaptureDevices();

    PHONONEXPERIMENTAL_EXPORT bool getHideAdvancedDevices();
    PHONONEXPERIMENTAL_EXPORT void hideAdvancedDevices(bool hide = true);

    /**
    * Returns the ordered list of audio output devices for a given category.
    *
    * \return An ordered list of AudioOutputDevice objects that give a name and
    * description for every supported audio output device.
    */
    PHONONEXPERIMENTAL_EXPORT QList<AudioOutputDevice> availableAudioOutputDevicesForCategory(Phonon::Category category);

    /**
    * Returns the ordered list of audio capture devices for a given category.
    *
    * \return An ordered list of AudioCaptureDevice objects that give a name and
    * description for every supported audio capture device.
    */
    PHONONEXPERIMENTAL_EXPORT QList<AudioCaptureDevice> availableAudioCaptureDevicesForCategory(Phonon::Category category);

    /**
    * Apply a priority list of output devices for a given category.
    *
    * \return An ordered list of device indexes that for the given category
    */
    PHONONEXPERIMENTAL_EXPORT void setAudioOutputDevicePriorityListForCategory(Phonon::Category category, QList<int> devices);

    /**
    * Apply a priority list of capture devices for a given category.
    *
    * \return An ordered list of device indexes that for the given category
    */
    PHONONEXPERIMENTAL_EXPORT void setAudioCaptureDevicePriorityListForCategory(Phonon::Category category, QList<int> devices);

} // namespace BackendCapabilities
} // namespace Experimental
} // namespace Phonon
#endif // PHONON_EXPERIMENTAL_BACKENDCAPABILITIES_H
