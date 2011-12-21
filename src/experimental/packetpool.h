/*  This file is part of the KDE project
    Copyright (C) 2007-2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_PACKETPOOL_H
#define PHONON_PACKETPOOL_H

#include "export.h"

namespace Phonon
{

class Packet;
class PacketPoolPrivate;
/** \class PacketPool packetpool.h phonon/PacketPool
 * \brief Class to preallocate memory.
 *
 * \note PacketPool and Packet are threadsafe.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONONEXPERIMENTAL_EXPORT PacketPool
{
    Q_DECLARE_PRIVATE(PacketPool)
    friend class Packet;
    public:
        /**
         * Allocates \p numberOfPackets packets of \p packetSize bytes each. The memory can be
         * accessed through Packet objects.
         */
        PacketPool(int packetSize, int numberOfPackets);
        /**
         * Copy constructor. Copying is fast since the class is explicitly shared.
         */
        PacketPool(const PacketPool &);
        /**
         * Destructs this object. If this is the last reference to the pool the memory will be
         * freed.
         */
        ~PacketPool();
        /**
         * Assignmen operator. Copying is fast since the class is explicitly shared.
         */
        PacketPool &operator=(const PacketPool &);

        /**
         * Returns the packet size that was set in the constructor.
         */
        int packetSize() const;
        /**
         * Returns the number of packets that was requested in the constructor.
         */
        int poolSize() const;
        /**
         * Returns the number of packets that are still available for use.
         */
        int unusedPackets() const;

    private:
        PacketPoolPrivate *d_ptr;
};

} // namespace Phonon
#endif // PHONON_PACKETPOOL_H
