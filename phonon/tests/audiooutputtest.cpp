/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "loadfakebackend.h"
#include "../audiooutput.h"
#include "../phononnamespace.h"

#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class AudioOutputTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void checkName();
        void checkVolume();
        void checkMute();
        void checkCategory();
        void cleanupTestCase();
};

using namespace Phonon;

void AudioOutputTest::initTestCase()
{
    QCoreApplication::setApplicationName("audiooutputtest");
    Phonon::loadFakeBackend();
}

void AudioOutputTest::checkName()
{
    AudioOutput ao(Phonon::NotificationCategory, this);
    QCOMPARE(ao.name(), QLatin1String("audiooutputtest"));
    QString n("lsdf");
    ao.setName(n);
    QCOMPARE(ao.name(), n);
}

void AudioOutputTest::checkVolume()
{
    AudioOutput ao(Phonon::NotificationCategory, this);
    QSignalSpy volumeSignalSpy(&ao, SIGNAL(volumeChanged(qreal)));
    qreal v = 1.0;
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(volumeSignalSpy.size(), 0);
    for (v = 0.0; v <= 10.0; v += 0.01)
    {
        ao.setVolume(v);
        QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
        QCOMPARE(volumeSignalSpy.size(), 1);
        QCOMPARE(qvariant_cast<float>(volumeSignalSpy.takeFirst().at(0)), static_cast<float>(v));
    }
    ao.setVolume(1.0); // the last value is what gets stored and restored next time
}

void AudioOutputTest::checkMute()
{
    AudioOutput ao(Phonon::CommunicationCategory, this);
    QSignalSpy volumeSignalSpy(&ao, SIGNAL(volumeChanged(qreal)));
    QSignalSpy muteSignalSpy(&ao, SIGNAL(mutedChanged(bool)));
    qreal v = 1.0;
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), false);

    ao.setMuted(true);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 1);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), true);
    QCOMPARE(muteSignalSpy.takeFirst().at(0).toBool(), true);

    ao.setMuted(true);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 0);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), true);

    ao.setMuted(false);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 1);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), false);
    QCOMPARE(muteSignalSpy.takeFirst().at(0).toBool(), false);

    ao.setMuted(false);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 0);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), false);

    ao.setMuted(true);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 1);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), true);
    QCOMPARE(muteSignalSpy.takeFirst().at(0).toBool(), true);

    v = 0.25f;
    ao.setVolume(v);
    QCOMPARE(volumeSignalSpy.size(), 1);
    QCOMPARE(muteSignalSpy.size(), 0);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), true);
    QCOMPARE(qvariant_cast<float>(volumeSignalSpy.takeFirst().at(0)), static_cast<float>(v));

    ao.setMuted(true);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 0);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), true);

    ao.setMuted(false);
    QCOMPARE(volumeSignalSpy.size(), 0);
    QCOMPARE(muteSignalSpy.size(), 1);
    QCOMPARE(static_cast<float>(ao.volume()), static_cast<float>(v));
    QCOMPARE(ao.isMuted(), false);
    QCOMPARE(muteSignalSpy.takeFirst().at(0).toBool(), false);

    ao.setVolume(1.0); // the last value is what gets stored and restored next time
}

void AudioOutputTest::checkCategory()
{
    for (int i = 0; i <= Phonon::LastCategory; ++i) {
        Phonon::Category cat = static_cast<Phonon::Category>(i);
        AudioOutput ao(cat, this);
        QCOMPARE(ao.category(), cat);
    }
}

void AudioOutputTest::cleanupTestCase()
{
}

QTEST_MAIN(AudioOutputTest)
#include "audiooutputtest.moc"
// vim: sw=4 ts=4
