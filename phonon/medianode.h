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

#ifndef PHONON_FRONTEND_H
#define PHONON_FRONTEND_H

#include "phonondefs.h"
#include "phonon_export.h"

namespace Phonon {

class FrontendPrivate;

class PHONON_EXPORT Frontend
{
public:
    virtual ~Frontend();

    /**
     * Tells whether the backend provides an implementation of this
     * class.
     *
     * \return \c true if backend provides an implementation
     * \return \c false if the object is not implemented by the backend
     */
    bool isValid() const;

protected:
    Frontend(FrontendPrivate &dd);
    FrontendPrivate *const k_ptr;

private:
    P_DECLARE_PRIVATE(Frontend)
};

} // namespace Phonon

#endif // PHONON_FRONTEND_H
