/*
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_FRONTEND_P_H
#define PHONON_FRONTEND_P_H

#include <QtCore/QtGlobal>
#include <QtCore/QList>
#include <QtCore/QObject>

#include "phononpimpl_p.h"
#include "phonon_export.h"

class QObject;

namespace Phonon {

class Frontend;

class PHONON_EXPORT FrontendPrivate
{
    friend class FactoryPrivate;

public:
    /**
     * Returns the backend object. If the object does not exist it tries to
     * create it before returning.
     *
     * \return the Iface object, might return \c 0
     */
    QObject *backendObject();

protected:
    FrontendPrivate();

    virtual ~FrontendPrivate();

    /**
     * \internal
     * This method cleanly deletes the Iface object. It is called on
     * destruction and before a backend change.
     */
    void deleteBackendObject();

    /**
     * \internal
     * Creates the Iface object belonging to this class. For most cases the
     * implementation is
     * \code
     * Q_Q(ClassName);
     * m_iface = Factory::createClassName(this);
     * return m_iface;
     * \endcode
     *
     * This function should not be called except from slotCreateIface.
     *
     * \see slotCreateIface
     */
    virtual void createBackendObject() = 0;

    virtual bool aboutToDeleteBackendObject();

protected:
    Frontend *q_ptr;
public:
    QObject *m_backendObject;

private:
    P_DECLARE_PUBLIC(Frontend)
    Q_DISABLE_COPY(FrontendPrivate)
};

} // namespace Phonon

#endif // PHONON_FRONTEND_P_H
