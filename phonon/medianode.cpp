/*
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). <thierry.bastian@trolltech.com>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#include "medianode.h"
#include "medianode_p.h"
#include "factory_p.h"

namespace Phonon {

FrontendPrivate::FrontendPrivate()
    : m_backendObject(0)
{
    Factory::registerFrontendObject(this);
}

FrontendPrivate::~FrontendPrivate()
{
    Factory::deregisterFrontendObject(this);
    delete m_backendObject;
    m_backendObject = 0;
}

QObject *FrontendPrivate::backendObject()
{
    if (!m_backendObject && Factory::backend()) {
        createBackendObject();
    }
    return m_backendObject;
}

void FrontendPrivate::deleteBackendObject()
{
    if (m_backendObject && aboutToDeleteBackendObject()) {
        delete m_backendObject;
        m_backendObject = 0;
    }
}

bool FrontendPrivate::aboutToDeleteBackendObject()
{
    return true;
}

Frontend::Frontend(FrontendPrivate &dd)
    : k_ptr(&dd)
{
    k_ptr->q_ptr = this;
}

bool Frontend::isValid() const
{
    return const_cast<FrontendPrivate *>(k_ptr)->backendObject() != 0;
}

Frontend::~Frontend()
{
    delete k_ptr;
}

} // namespace Phonon
