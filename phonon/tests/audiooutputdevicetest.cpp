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

#include "../objectdescription.h"
#include "loadfakebackend.h"

#include <QtCore/QObject>

#include <QtTest/QTest>

class AudioOutputDeviceTest : public QObject
{
    Q_OBJECT
    private Q_SLOTS:
        void initTestCase();
        void sensibleValues();
        void cleanupTestCase();
};

using namespace Phonon;

void AudioOutputDeviceTest::initTestCase()
{
    QCoreApplication::setApplicationName("audiooutputdevicetest");
    Phonon::loadFakeBackend();
}

void AudioOutputDeviceTest::sensibleValues()
{
    AudioOutputDevice a;
    QCOMPARE(a.isValid(), false);
    AudioOutputDevice b(a);
    QCOMPARE(b.isValid(), false);
    b = a;
    QCOMPARE(b.isValid(), false);

    if (Factory::backendName() == QLatin1String("Fake")) {
        AudioOutputDevice c = AudioOutputDevice::fromIndex(10000);
        QCOMPARE(c.isValid(), true);
        QCOMPARE(c.index(), 10000);
        QCOMPARE(c.name(), QString("internal Soundcard"));
        QCOMPARE(c.description(), QString());
        b = AudioOutputDevice::fromIndex(10001);
        QCOMPARE(b.isValid(), true);
        QCOMPARE(b.index(), 10001);
        QCOMPARE(b.name(), QString("USB Soundcard"));
        QCOMPARE(b.description(), QString());
        QCOMPARE(a.isValid(), false);
        a = c;
        QCOMPARE(a, c);
        QCOMPARE(a.isValid(), true);
        QCOMPARE(a.index(), 10000);
        QCOMPARE(a.name(), QString("internal Soundcard"));
        QCOMPARE(a.description(), QString());
    }
}

void AudioOutputDeviceTest::cleanupTestCase()
{
}

QTEST_MAIN(AudioOutputDeviceTest)
#include "audiooutputdevicetest.moc"
// vim: sw=4 ts=4
