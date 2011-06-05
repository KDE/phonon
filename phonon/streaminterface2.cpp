/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#include "streaminterface2.h"
#include "streaminterface_p.h"

#include "abstractmediastream2.h"
#include "phonondefs_p.h"

namespace Phonon {

class StreamInterface2Private : public StreamInterfacePrivate
{
    friend class StreamInterface2;
public:
    inline StreamInterface2Private(StreamInterface2 *q_) :
        StreamInterfacePrivate(q_)
    {
    }

    inline ~StreamInterface2Private() {}
};

StreamInterface2::StreamInterface2() :
    StreamInterface(*(new StreamInterface2Private(this)))
{
}

StreamInterface2::~StreamInterface2()
{
}

void StreamInterface2::needData(qint64 size)
{
    K_D(StreamInterface2);
    if (d->mediaSource.type() == MediaSource::Stream) {
        QMetaObject::invokeMethod(d->mediaSource.stream(), "needData",
                                  Qt::QueuedConnection, Q_ARG(qint64, size));
    }
}

} // namespace Phonon
