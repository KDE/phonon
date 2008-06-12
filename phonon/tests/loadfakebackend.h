/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TESTS_LOADFAKEBACKEND_H
#define TESTS_LOADFAKEBACKEND_H

#include "../factory_p.h"
#include <QtCore/QUrl>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <cstdlib>

#ifdef USE_FAKE_BACKEND
#include "fakebackend/backend.h"
#endif

namespace Phonon
{
void loadFakeBackend()
{
#ifdef USE_FAKE_BACKEND
    Factory::setBackend(new Fake::Backend);
#endif
}
QUrl testUrl()
{
#ifdef USE_FAKE_BACKEND
    return QUrl("file:///foo.ogg");
#else
    QUrl url(std::getenv("PHONON_TESTURL"));
    if (!url.isValid()) {
        url = QUrl::fromLocalFile(std::getenv("PHONON_TESTURL"));
        if (!url.isValid()) {
            QWARN("You need to set PHONON_TESTURL to a valid URL. Expect to see failures.");
        }
    }
    return url;
#endif
}
} // namespace Phonon

#endif // TESTS_LOADFAKEBACKEND_H
