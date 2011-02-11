/*  This file is part of the KDE project
    Copyright © 2008 Matthias Kretz <kretz@kde.org>
    Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>

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
#ifndef PHONON_EXPERIMENTAL_OBJECTDESCRIPTION_H
#define PHONON_EXPERIMENTAL_OBJECTDESCRIPTION_H

#include "../objectdescription.h"

namespace Phonon
{
namespace Experimental
{

#ifndef PHONON_NO_VIDEOCAPTURE

enum ObjectDescriptionType
{
    VideoCaptureDeviceType = 0x10000
};

typedef Phonon::ObjectDescription<static_cast<Phonon::ObjectDescriptionType>(Phonon::Experimental::VideoCaptureDeviceType)> VideoCaptureDevice;

VideoCaptureDevice phononVcdToExperimentalVcd(const Phonon::VideoCaptureDevice &vcd);

/**
 * Generates a Phonon::VideoCaptureDevice from an equal object of
 * the Phonon::Experimental namespace.
 * 
 * @arg vcd the VideoCaptureDevice to take as reference
 * @return a Phonon::VideoCaptureDevice instance with the same properties as the incoming vcd
 */
Phonon::VideoCaptureDevice phononExperimentalVcdToVcd(const Phonon::Experimental::VideoCaptureDevice &vcd);

#endif // PHONON_NO_VIDEOCAPTURE

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_EXPERIMENTAL_OBJECTDESCRIPTION_H
