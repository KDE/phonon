
#define QT_GUI_LIB

#include <stdlib.h>
#include <Qt/qobject.h>
#include <Qt/qwidget.h>
#include <Qt/qdebug.h>
#include <QtTest/QTest>
#include <phonon/Global>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/VideoWidget>
#include <experimental/globalconfig.h>

using namespace std;
using namespace Phonon::Experimental;

class AVCaptureTest : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testDeviceList();
        void cleanupTestCase();
    private:
        Phonon::MediaObject *m_media;
        Phonon::AudioOutput *m_aout;
        Phonon::VideoWidget *m_vwid;
};


void AVCaptureTest::initTestCase()
{
    m_media = new Phonon::MediaObject();
    m_aout = new Phonon::AudioOutput(Phonon::VideoCategory, NULL);
    m_vwid = new Phonon::VideoWidget(NULL);

    Phonon::createPath(m_media, m_vwid);
    Phonon::createPath(m_media, m_aout);
}


void AVCaptureTest::cleanupTestCase()
{
    delete m_aout;
    delete m_vwid;
    delete m_media;
}


void AVCaptureTest::testDeviceList()
{
    // Write info
    Phonon::Experimental::GlobalConfig pgc;
    qDebug() << "Device list for audio capture music" << pgc.audioCaptureDeviceListFor(Phonon::MusicCategory);
    qDebug() << "Device list for video capture" << pgc.videoCaptureDeviceListFor(Phonon::NoCategory);
}


QTEST_MAIN(AVCaptureTest)

#include "avcapturetest.moc"
