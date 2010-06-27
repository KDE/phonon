/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef AVCAPTURE_P_H
#define AVCAPTURE_P_H

#ifndef NO_PHONON_AVCAPTURE

#include "avcapture.h"
#include "../medianode_p.h"
#include "../phonondefs_p.h"

namespace Phonon
{
namespace Experimental
{

class AvCapturePrivate : public MediaNodePrivate
{
    Q_DECLARE_PUBLIC(AvCapture)
    PHONON_PRIVATECLASS
    public:
    protected:
        AudioCaptureDevice audioCaptureDevice;
        VideoCaptureDevice videoCaptureDevice;
};

} // namespace Experimental
} // namespace Phonon

#endif // NO_PHONON_AVCAPTURE

#endif // AVCAPTURE_P_H
// vim: sw=4 ts=4 tw=80
