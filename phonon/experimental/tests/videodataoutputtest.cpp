/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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

#define QT_GUI_LIB
#include "../abstractvideodataoutput.h"
#include "../videoframe2.h"
#include "../../mediaobject.h"
#include "../../tests/qtesthelper.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QImageReader>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtTest/QtTest>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

class Output : public QObject, public Phonon::Experimental::AbstractVideoDataOutput
{
    Q_OBJECT
    public:
        Output() : receivedEnd(0) {}
        virtual QSet<Phonon::Experimental::VideoFrame2::Format> allowedFormats() const;
        virtual void frameReady(const Phonon::Experimental::VideoFrame2 &);
        virtual void endOfMedia();

        QList<QImage> frames;
        int receivedEnd;
        QMutex mutex;
};

QSet<Phonon::Experimental::VideoFrame2::Format> Output::allowedFormats() const
{
    return QSet<Phonon::Experimental::VideoFrame2::Format>() << Phonon::Experimental::VideoFrame2::Format_RGB888;
}

void Output::frameReady(const Phonon::Experimental::VideoFrame2 &frame)
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(frame.format == Phonon::Experimental::VideoFrame2::Format_RGB888);
    QMutexLocker lock(&mutex);
    frames << frame.qImage();
}

void Output::endOfMedia()
{
    ++receivedEnd;
}

class VideoDataOutputTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testDelete();
        void playMedia();
        void cleanupTestCase();
    private:
        Phonon::MediaObject *m_media;
        Output *m_output;
};

void VideoDataOutputTest::initTestCase()
{
    m_media = new Phonon::MediaObject;
    m_output = new Output;
    Phonon::createPath(m_media, m_output);
}

void VideoDataOutputTest::testDelete()
{
    delete m_output;
    m_output = new Output;
    m_output->start();
    QVERIFY(m_output->isRunning());
    Phonon::createPath(m_media, m_output);
#ifdef Q_OS_WIN
    Sleep(1000);
#else
    sleep(1);
#endif
}

void VideoDataOutputTest::playMedia()
{
    m_media->setCurrentSource(QString(":/test.mng"));
    m_media->play();
    QVERIFY(QTest::kWaitForSignal(m_media, SIGNAL(finished()), 4000));
    QEXPECT_FAIL("", "endOfMedia not yet implemented for xine", Continue);
    QCOMPARE(m_output->receivedEnd, 1);
    QList<QImage> frames;
    {
        QImageReader movie(":/test.mng");
        QImage frame;
        while (movie.read(&frame)) {
            frames << frame.convertToFormat(QImage::Format_RGB888);
        }
    }
    qDebug() << frames.size();
    QMutexLocker lock(&m_output->mutex);
    QVERIFY(!m_output->frames.isEmpty());
    QCOMPARE(m_output->frames.size(), frames.size());
    for (int i = 0; i < frames.size(); ++i) {
        QCOMPARE(m_output->frames[i], frames[i]);
    }
}

void VideoDataOutputTest::cleanupTestCase()
{
    delete m_output;
    delete m_media;
}

QTEST_MAIN(VideoDataOutputTest)

#include "videodataoutputtest.moc"
