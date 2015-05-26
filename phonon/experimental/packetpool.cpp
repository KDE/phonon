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

#include "packetpool.h"
#include "packetpool_p.h"
#include "packet.h"
#include "packet_p.h"

namespace Phonon
{

int PacketPool::packetSize() const { return d_ptr->packetSize; }
int PacketPool::poolSize() const { return d_ptr->poolSize; }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
int PacketPool::unusedPackets() const { return d_ptr->ringBufferSize.loadAcquire(); }
#else
int PacketPool::unusedPackets() const { return d_ptr->ringBufferSize; }
#endif

PacketPoolPrivate::PacketPoolPrivate(int _packetSize, int _poolSize)
    : freePackets(new PacketPrivate *[_poolSize]),
    packetMemory(new char[_packetSize * _poolSize]),
    readPosition(0), writePosition(0),
    ringBufferSize(_poolSize),
    packetSize(_packetSize),
    poolSize(_poolSize)
{
    for (int i = 0; i < _poolSize; ++i) {
        freePackets[i] = new PacketPrivate(&packetMemory[i * packetSize], this);
    }
}

PacketPoolPrivate::~PacketPoolPrivate()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_ASSERT(poolSize == ringBufferSize.loadAcquire());
#else
    Q_ASSERT(poolSize == ringBufferSize);
#endif
    for (int i = 0; i < poolSize; ++i) {
        delete freePackets[i];
    }
    delete[] freePackets;
    delete[] packetMemory;
}

void PacketPoolPrivate::releasePacket(const Packet &packet)
{
    const int _writePos = writePosition.fetchAndAddAcquire(1);
    int pos = _writePos;
    while (pos >= poolSize) {
        pos -= poolSize;
    }
    writePosition.testAndSetRelease(_writePos, pos);
    freePackets[pos] = packet.d_ptr;
    ringBufferSize.ref();
}

Packet PacketPoolPrivate::acquirePacket()
{
    const int s = ringBufferSize.fetchAndAddRelaxed(-1);
    if (s <= 0) {
        ringBufferSize.fetchAndAddRelaxed(1);
        return Packet();
    }
    const int _readPos = readPosition.fetchAndAddRelaxed(1);
    int pos = _readPos;
    while (pos >= poolSize) {
        pos -= poolSize;
    }
    readPosition.testAndSetRelease(_readPos, pos);
    freePackets[pos]->m_size = 0;
    return Packet(*freePackets[pos]);
}

PacketPool::PacketPool(int packetSize, int _poolSize)
    : d_ptr(new PacketPoolPrivate(packetSize, _poolSize))
{
    d_ptr->ref.ref();
}

PacketPool::PacketPool(const PacketPool &rhs)
    : d_ptr(rhs.d_ptr)
{
    d_ptr->ref.ref();
}

PacketPool &PacketPool::operator=(const PacketPool &rhs)
{
    if (d_ptr != rhs.d_ptr) {
        if (!d_ptr->ref.deref()) {
            delete d_ptr;
        }
        d_ptr = rhs.d_ptr;
        d_ptr->ref.ref();
    }
    return *this;
}

PacketPool::~PacketPool()
{
    if (!d_ptr->ref.deref()) {
        delete d_ptr;
    }
}

} // namespace Phonon
