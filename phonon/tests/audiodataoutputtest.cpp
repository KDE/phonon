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

#include "../audiodataoutput.h"
#include "../mediaobject.h"
#include "../path.h"
#include "../audiooutput.h"
#include "loadfakebackend.h"

#include <QtTest/QTest>
#include <cstdlib>
#include <QtCore/QUrl>
#include <QtCore/QObject>

class AudioDataOutputTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testSampleRate();
        void testDataSize();
        void cleanupTestCase();
};

using namespace Phonon;

void AudioDataOutputTest::initTestCase()
{
    QCoreApplication::setApplicationName("audiodataoutputtest");
    Phonon::loadFakeBackend();
    AudioDataOutput test(this);
}

void AudioDataOutputTest::testSampleRate()
{
    AudioDataOutput test(this);
    QVERIFY(test.sampleRate() > 0);
}

void AudioDataOutputTest::testDataSize()
{
    AudioDataOutput test(this);
    QVERIFY(test.dataSize() > 0);
}

void AudioDataOutputTest::cleanupTestCase()
{
}

QTEST_MAIN(AudioDataOutputTest)
#include "audiodataoutputtest.moc"
