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

#ifndef PHONON_PACKET_H
#define PHONON_PACKET_H

#include "export.h"

namespace Phonon
{

class PacketPool;

struct PacketPrivate;
/** \class Packet packetpool.h phonon/Packet
 * \brief Class to access memory preallocated by PacketPool
 *
 * \note PacketPool and Packet are threadsafe.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONONEXPERIMENTAL_EXPORT Packet
{
    friend class PacketPoolPrivate;
    Q_DECLARE_PRIVATE(Packet)
    public:
        /**
         * Constructs a null packet.
         *
         * \see isNull
         */
        Packet();
        /**
         * Returns a packet with a capacity of pool.packetSize if there is still free data in the
         * PacketPool. Returns a null packet otherwise. The size will initially be set to 0.
         */
        explicit Packet(PacketPool &pool);
        /**
         * Returns a shared copy of the object. Note that Packet will not detach (and it can not
         * detach as there's a fixed amount of memory preallocated. If you want to copy the actual
         * memory data you have to request another packet from the pool and copy the memory
         * yourself.)
         */
        Packet(const Packet &rhs);
        /**
         * Assigns a shared copy of the object. Note that Packet will not detach (and it can not
         * detach as there's a fixed amount of memory preallocated. If you want to copy the actual
         * memory data you have to request another packet from the pool and copy the memory
         * yourself.)
         */
        Packet &operator=(const Packet &rhs);

        /**
         * Dereferences the packet data. If this is the last reference that gets released the packet
         * becomes available in the PacketPool again automatically.
         */
        ~Packet();

        /**
         * Returns whether the packets reference the same data.
         */
        bool operator==(const Packet &rhs) const;

        /**
         * Returns whether the packets reference different data.
         */
        bool operator!=(const Packet &rhs) const;

        /**
         * Returns whether this object is a null packet.
         *
         * \see Packet()
         */
        bool isNull() const;

        /**
         * Returns a pointer to read the data this packet references.
         *
         * You may read size() bytes.
         */
        const char *data() const;

        /**
         * Returns a pointer to read and write the data this packet references.
         *
         * You may read size() bytes.
         * You may write capacity() bytes.
         * If you write to this pointer do not forget to adjust the size by calling setSize().
         */
        char *data();

        /**
         * Returns the number of bytes that have a defined value.
         */
        int size() const;

        /**
         * Sets how many bytes in the data pointer have a defined value.
         */
        void setSize(int size);

        /**
         * Returns the number of bytes that may be accessed.
         */
        int capacity() const;

    protected:
        explicit Packet(PacketPrivate &dd);
        PacketPrivate *d_ptr;
};

} // namespace Phonon
#endif // PHONON_PACKET_H
