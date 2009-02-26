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

#ifndef PHONON_EXPERIMENTAL_MEDIASOURCE_H
#define PHONON_EXPERIMENTAL_MEDIASOURCE_H

#include "../mediasource.h"
#include "export.h"
#include "objectdescription.h"

namespace Phonon
{
namespace Experimental
{

class PHONONEXPERIMENTAL_EXPORT MediaSource : public Phonon::MediaSource
{
    public:
        enum Type {
            Link = 0xffff,
            VideoCaptureDeviceSource
        };

        /**
         * Constructs a copy of \p rhs.
         *
         * This constructor is fast thanks to explicit sharing.
         */
        MediaSource(const MediaSource &rhs);

        /**
         * Assigns \p rhs to this MediaSource and returns a reference to this MediaSource.
         *
         * This operation is fast thanks to explicit sharing.
         */
        MediaSource &operator=(const MediaSource &rhs);

        /**
         * Returns \p true if this MediaSource is equal to \p rhs; otherwise returns \p false.
         */
        bool operator==(const MediaSource &rhs) const;

        VideoCaptureDevice videoCaptureDevice() const;

        MediaSource(const VideoCaptureDevice &videoDevice);
        MediaSource(const QList<Phonon::MediaSource> &mediaList);

        QList<Phonon::MediaSource> substreams() const;
};

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_EXPERIMENTAL_MEDIASOURCE_H
