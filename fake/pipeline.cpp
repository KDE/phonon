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

#include "pipeline.h"

namespace Phonon {
namespace Fake {

Pipeline::Pipeline()
{
    reset();
}

Pipeline::~Pipeline() {}

qint64 Pipeline::time() const
{
    return m_time;
}

void Pipeline::start()
{
    connect(this, SIGNAL(timeout()), SLOT(onTimeout()));
    QTimer::start();
}

void Pipeline::pause()
{
    disconnect(this);
}

void Pipeline::stop()
{
    pause();
    QTimer::stop();
    reset();
}

void Pipeline::reset()
{
    m_time = 0;
}

void Pipeline::seek(qint64 msec)
{
    m_time = msec;
}

void Pipeline::onTimeout()
{
    m_time += interval();
}

} // namespace Fake
} // namespace Phonon
