/*  This file is part of the KDE project
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

#include "../mrl.h"

#include <QtCore/QDebug>
#include <QtTest/QTest>

class MrlTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testDCtor();
    void testCCtor();
    void testQUrlCtor();
    void testQStringCtor();
    void testAssignment();
#ifndef QT_NO_URL_CAST_FROM_STRING
    void testQStringAssingment();
#endif

    void testLocalEncodingWithoutScheme();
    void testLocalEncodingWithScheme();
    void testUrlEncoding();

    void testImplicitSharing();

    void cleanupTestCase();
};

using namespace Phonon;

void MrlTest::initTestCase()
{
}

void MrlTest::testDCtor()
{
    Mrl mrl;
    QCOMPARE(mrl.isValid(), false);
    QCOMPARE(mrl.toString(), QString());
}

void MrlTest::testCCtor()
{
    Mrl mrl(QString("/usr/share/sounds/KDE_Beep.ogg"));

    Mrl newMrl(mrl);

    QCOMPARE(newMrl.isValid(), mrl.isValid());
    QCOMPARE(newMrl.toString(), mrl.toString());
}

void MrlTest::testQUrlCtor()
{
    QUrl qurl(QString("/usr/share/sounds/KDE_Beep.ogg"));

    Mrl mrl(qurl);

    QCOMPARE(mrl.isValid(), qurl.isValid());
    QCOMPARE(mrl.toString(), qurl.toString());
}

void MrlTest::testQStringCtor()
{
    QString url("/usr/share/sounds/KDE_Beep.ogg");
    QUrl qurl(url);

    Mrl mrl(url);

    QCOMPARE(mrl.isValid(), qurl.isValid());
    QCOMPARE(mrl.toString(), qurl.toString());
    QCOMPARE(mrl.toString(), url);
}

void MrlTest::testAssignment()
{
    Mrl mrl(QString("/usr/share/sounds/KDE_Beep.ogg"));

    Mrl newMrl = mrl;

    QCOMPARE(newMrl.isValid(), mrl.isValid());
    QCOMPARE(newMrl.toString(), mrl.toString());
}

#ifndef QT_NO_URL_CAST_FROM_STRING
void MrlTest::testQStringAssingment()
{
    QString filename("/usr/share/sounds/KDE_Beep.ogg");

    Mrl mrl;
    mrl = filename;

    QCOMPARE(mrl.toString(), filename);
}
#endif

void MrlTest::testLocalEncodingWithoutScheme()
{
    QString url("/home/test/Geräteüberhöhung.mp3");
    Mrl mrl(url);
    QUrl qurl(url);
    qurl.setScheme(QString("file://"));

    qDebug() << "MRL: " << mrl.toEncoded();
    qDebug() << "QURL: " << qurl.toEncoded();

    QEXPECT_FAIL("", "QUrl produces expected bogus encoded", Continue);
    QCOMPARE(mrl.toEncoded(), qurl.toEncoded());
    QCOMPARE(mrl.toEncoded(), QByteArray("file:///home/test/Ger%C3%A4te%C3%BCberh%C3%B6hung.mp3"));
}

void MrlTest::testLocalEncodingWithScheme()
{
    QString url("file:///home/test/Geräteüberhöhung.mp3");
    Mrl mrl(url);
    QUrl qurl(url);

    QEXPECT_FAIL("", "QUrl produces expected bogus encoded", Continue);
    QCOMPARE(mrl.toEncoded(), qurl.toEncoded());
    QCOMPARE(mrl.toEncoded(), QByteArray("file:///home/test/Ger%C3%A4te%C3%BCberh%C3%B6hung.mp3"));
}

void MrlTest::testUrlEncoding()
{
    QString url("http://user@häst.eu/a/b/c/d/Geräteüberhöhung.mp3");
    Mrl mrl(url);
    QUrl qurl(url);

    QCOMPARE(mrl.toEncoded(), qurl.toEncoded());
}

void MrlTest::testImplicitSharing()
{
    QString url("/usr/share/sounds/KDE_Beep.ogg");
    QUrl qurl(QString("/usr/share/sounds/KDE_Beep.ogg"));
    Mrl mrl(qurl);

    mrl.setHost(QString("randomhost.com"));
    mrl.setScheme(QString("http"));

    QCOMPARE(mrl.toString(), QString("http://randomhost.com/usr/share/sounds/KDE_Beep.ogg"));
    QCOMPARE(qurl.toString(), url);
}

void MrlTest::cleanupTestCase()
{
}

QTEST_APPLESS_MAIN(MrlTest)

#include "mrltest.moc"
