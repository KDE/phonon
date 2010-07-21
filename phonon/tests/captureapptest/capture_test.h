#ifndef PHONON_TEST_H
#define PHONON_TEST_H

#include <Qt/QtCore>
#include <Qt/QtGui>
#include <backendcapabilities.h>
#include <objectdescription.h>
#include <audiooutput.h>
#include <videoplayer.h>
#include <videowidget.h>
#include <volumeslider.h>
#include <mediaobject.h>
#include <globalconfig.h>
#include <objectdescriptionmodel.h>

class MediaPlayer : public QWidget
{
    Q_OBJECT

    public:
        MediaPlayer(QWidget *parent);
        ~MediaPlayer();

    public slots:
        void setDeviceIndex(int index);

    private:
        void updateDeviceList();

    private:
        Phonon::MediaObject *m_media;
        Phonon::AudioOutput *m_aoutput;
        Phonon::VideoWidget *m_vwidget;
        Phonon::VolumeSlider *m_volumeSlider;
        Phonon::VideoCaptureDeviceModel *m_deviceModel;

        QPushButton *m_playButton;
        QPushButton *m_pauseButton;
        QPushButton *m_stopButton;
        QComboBox *m_deviceNameCombo;
};


#endif



