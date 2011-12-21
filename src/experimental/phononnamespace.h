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

#ifndef PHONON_X_PHONONNAMESPACE_H
#define PHONON_X_PHONONNAMESPACE_H

#include "../phononnamespace.h"

namespace Phonon
{
namespace Experimental
{

    enum BitRate {
        UnknownBitRate = -1,
        /**
         * Audio Data is transported as values from -128 to 127.
         */
        Signed8Bit = 100,
        /**
         * Audio Data is transported as values from 0 to 255
         */
        Unsigned8Bit = 200,
        /**
         * Audio Data is transported as values from -2^15 to 2^15-1
         */
        Signed16Bit = 300,
        Signed18Bit = 400,
        Signed20Bit = 500,
        /**
         * Audio Data is transported as values from -2^23 to 2^23-1. The data is packed in 3
         * Bytes and not padded with a 0 Byte.
         */
        Signed24Bit = 600,
        /**
         * Audio Data is transported as values from -2^31 to 2^31-1
         */
        Signed32Bit = 700,
        /**
         * Audio Data is transported as values from -1.0 to 1.0
         */
        Float32Bit = 800
    };

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_X_PHONONNAMESPACE_H
