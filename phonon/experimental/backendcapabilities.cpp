/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), Nokia Corporation (or its successors,
    if any) and the KDE Free Qt Foundation, which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "backendcapabilities.h"
#include "../backendcapabilities.h"
#include "globalconfig.h"

namespace Phonon
{
namespace Experimental
{

#ifndef PHONON_NO_VIDEOCAPTURE
QList<VideoCaptureDevice> BackendCapabilities::availableVideoCaptureDevices()
{
    QList<Phonon::VideoCaptureDevice> phononList;
    QList<VideoCaptureDevice> experimentalList;

    phononList = Phonon::BackendCapabilities::availableVideoCaptureDevices();
    foreach (const Phonon::VideoCaptureDevice &vcd, phononList) {
        experimentalList << phononVcdToExperimentalVcd(vcd);
    }

    return experimentalList;
}
#endif

} // namespace Experimental
} // namespace Phonon
