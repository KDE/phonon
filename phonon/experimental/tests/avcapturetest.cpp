
#define QT_GUI_LIB

#include <stdlib.h>
#include <Qt/qobject.h>
#include <Qt/qwidget.h>
#include <Qt/qlist.h>
#include <Qt/qdebug.h>
#include <QtTest/QTest>
#include <phonon/Global>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/VideoWidget>
#include <../globalconfig.h>

using namespace std;
using namespace Phonon::Experimental;

class AVCaptureTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testAudioOutput();
        void testAudioCapture();
        void testVideoCapture();
        void cleanupTestCase();
    private:
        Phonon::MediaObject *m_media;
        Phonon::AudioOutput *m_aout;
        Phonon::VideoWidget *m_vwid;
        Phonon::Experimental::GlobalConfig *m_pgc;
};


void AVCaptureTest::initTestCase()
{
    m_media = new Phonon::MediaObject();
    m_aout = new Phonon::AudioOutput(Phonon::VideoCategory, NULL);
    m_vwid = new Phonon::VideoWidget(NULL);

    Phonon::createPath(m_media, m_vwid);
    Phonon::createPath(m_media, m_aout);

    m_pgc = new Phonon::Experimental::GlobalConfig();
}


void AVCaptureTest::cleanupTestCase()
{
    delete m_pgc;
    delete m_aout;
    delete m_vwid;
    delete m_media;
}


void AVCaptureTest::testAudioOutput()
{
    // Write device indices
    QList<int> aoList = m_pgc->audioOutputDeviceListFor(Phonon::NoCategory);
    qDebug() << "Device list for audio output" << aoList;

    // Write device properties
    QHash<QByteArray, QVariant> info;
    int index;
    foreach(index, aoList)
    {
        info = m_pgc->audioOutputDeviceProperties(index);
        qDebug() << "Device properties for audio output" << index << ":";
        qDebug() << info;
        QVERIFY(!info.isEmpty());
    }
}


void AVCaptureTest::testAudioCapture()
{
    // Write device indices
    QList<int> acList = m_pgc->audioCaptureDeviceListFor(Phonon::NoCategory);
    qDebug() << "Device list for audio capture" << acList;

    // Write device properties
    QHash<QByteArray, QVariant> info;
    int index;
    foreach(index, acList)
    {
        info = m_pgc->audioCaptureDeviceProperties(index);
        qDebug() << "Device properties for audio capture" << index << ":";
        qDebug() << info;
        QVERIFY(!info.isEmpty());
    }
}


void AVCaptureTest::testVideoCapture()
{
    // Write device indices
    QList<int> acList = m_pgc->videoCaptureDeviceListFor(Phonon::NoCategory);
    qDebug() << "Device list for video capture" << acList;

    // Write device properties
    QHash<QByteArray, QVariant> info;
    int index;
    foreach(index, acList)
    {
        info = m_pgc->videoCaptureDeviceProperties(index);
        qDebug() << "Device properties for video capture" << index << ":";
        qDebug() << info;
        QVERIFY(!info.isEmpty());
    }
}


QTEST_MAIN(AVCaptureTest)

#include "avcapturetest.moc"
