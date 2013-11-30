/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Casian Andrei <skeletk13@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), Nokia Corporation
 * (or its successors, if any) and the KDE Free Qt Foundation, which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sourcecontrol.h"
#include "sourcecontrol_p.h"

#include "factory_p.h"

namespace Phonon
{


/*
 * SourceControl
 */

SourceControl::SourceControl(SourceControlPrivate &pd, QObject *parent)
    : QObject(parent)
    , Frontend(pd)
{

}

SourceControl::~SourceControl()
{

}

bool SourceControl::isActive() const
{
    P_D(const SourceControl);
    if (!d->m_scInterface)
        return false;
    return d->m_scInterface->isActive();
}

Source SourceControl::source() const
{
    P_D(const SourceControl);
    return d->m_source;
}

// private

SourceControlPrivate::SourceControlPrivate(Source &source)
    : m_scInterface(0)
    , m_source(source)
{

}

SourceControlPrivate::~SourceControlPrivate()
{

}

} // Phonon namespace
