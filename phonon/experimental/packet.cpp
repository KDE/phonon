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

#include "packet.h"
#include "packet_p.h"
#include "packetpool.h"
#include "packetpool_p.h"

namespace Phonon
{

Q_GLOBAL_STATIC(PacketPrivate, shared_null_packet)

Packet::Packet() : d_ptr(shared_null_packet()) { d_ptr->ref.ref(); }
Packet::Packet(PacketPool &pool) : d_ptr(pool.d_ptr->acquirePacket().d_ptr) { d_ptr->ref.ref(); }
Packet::Packet(const Packet &rhs) : d_ptr(rhs.d_ptr) { d_ptr->ref.ref(); }
Packet::Packet(PacketPrivate &dd) : d_ptr(&dd) { d_ptr->ref.ref(); }
Packet &Packet::operator=(const Packet &rhs)
{
    if (!d_ptr->ref.deref()) {
        Q_ASSERT(d_ptr->m_pool);
        d_ptr->m_pool->releasePacket(*this);
    }
    d_ptr = rhs.d_ptr;
    d_ptr->ref.ref();
    return *this;
}
Packet::~Packet()
{
    if (!d_ptr->ref.deref()) {
        Q_ASSERT(d_ptr->m_pool);
        d_ptr->m_pool->releasePacket(*this);
    }
}
bool Packet::operator==(const Packet &rhs) const { return d_ptr == rhs.d_ptr; }
bool Packet::operator!=(const Packet &rhs) const { return d_ptr != rhs.d_ptr; }
bool Packet::isNull() const { return d_ptr->m_pool == 0; }
const char *Packet::data() const { return d_ptr->m_data; }
char *Packet::data() { return d_ptr->m_data; }
int Packet::size() const { return d_ptr->m_size; }
void Packet::setSize(int size) { d_ptr->m_size = size; }
int Packet::capacity() const { return d_ptr->m_pool ? d_ptr->m_pool->packetSize : 0; }

} // namespace Phonon
