
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
    // Write device indices
    Phonon::Experimental::GlobalConfig pgc;
    QList<int> aoList = pgc.audioOutputDeviceListFor(Phonon::NoCategory);
    QList<int> acList = pgc.audioCaptureDeviceListFor(Phonon::NoCategory);
    QList<int> vcList = pgc.videoCaptureDeviceListFor(Phonon::NoCategory);
    qDebug() << "Device list for audio output" << aoList;
    qDebug() << "Device list for audio capture" << acList;
    qDebug() << "Device list for video capture" << vcList;

    // Write device properties
    QHash<QByteArray, QVariant> info;
    int index;
    foreach(index, aoList)
    {
        info = pgc.audioOutputDeviceProperties(index);
        qDebug() << "Device properties for audio output" << index << ":";
        qDebug() << info;
    }
}


QTEST_MAIN(AVCaptureTest)

#include "avcapturetest.moc"
