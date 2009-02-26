/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#include "loadfakebackend.h"
#include <QtCore/QObject>
#include <qtest_kde.h>
#include "../objectdescription.h"

class VideoCaptureDeviceTest : public QObject
{
    Q_OBJECT
    private Q_SLOTS:
        void initTestCase();
        void sensibleValues();
        void cleanupTestCase();
};

using namespace Phonon;

void VideoCaptureDeviceTest::initTestCase()
{
    QCoreApplication::setApplicationName("videocapturedevicetest");
    Phonon::loadFakeBackend();
}

void VideoCaptureDeviceTest::sensibleValues()
{
    VideoCaptureDevice a;
    QCOMPARE(a.isValid(), false);
    VideoCaptureDevice b(a);
    QCOMPARE(b.isValid(), false);
    b = a;
    QCOMPARE(b.isValid(), false);

    if (Factory::backendName() == QLatin1String("Fake")) {
        VideoCaptureDevice c = VideoCaptureDevice::fromIndex(30000);
        QCOMPARE(c.isValid(), true);
        QCOMPARE(c.index(), 30000);
        QCOMPARE(c.name(), QString("USB Webcam"));
        QCOMPARE(c.description(), QString("first description"));
        b = VideoCaptureDevice::fromIndex(30001);
        QCOMPARE(b.isValid(), true);
        QCOMPARE(b.index(), 30001);
        QCOMPARE(b.name(), QString("DV"));
        QCOMPARE(b.description(), QString("second description"));
        QCOMPARE(a.isValid(), false);
        a = c;
        QCOMPARE(a, c);
        QCOMPARE(a.isValid(), true);
        QCOMPARE(a.index(), 30000);
        QCOMPARE(a.name(), QString("USB Webcam"));
        QCOMPARE(a.description(), QString("first description"));
    //} else {
    }
}

void VideoCaptureDeviceTest::cleanupTestCase()
{
}

QTEST_MAIN(VideoCaptureDeviceTest)
#include "videocapturedevicetest.moc"
// vim: sw=4 ts=4
