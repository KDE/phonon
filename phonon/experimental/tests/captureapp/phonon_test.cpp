
#include "phonon_test.h"
#include <experimental/globalconfig.h>

MediaPlayer::MediaPlayer(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_vwidget = new Phonon::VideoWidget(this);
    m_vwidget->setMinimumSize(QSize(400, 300));
    layout->addWidget(m_vwidget);

    m_aoutput = new Phonon::AudioOutput();

    m_media = new Phonon::MediaObject();

    Phonon::createPath(m_media, m_aoutput);
    Phonon::createPath(m_media, m_vwidget);

    QHBoxLayout *urlLayout = new QHBoxLayout(this);

    m_deviceNameEdit = new QLineEdit(this);
    m_deviceNameEdit->setText("/dev/video0");
    urlLayout->addWidget(m_deviceNameEdit);
    connect(m_deviceNameEdit, SIGNAL(editingFinished()), this, SLOT(setDeviceName()));

    layout->addItem(urlLayout);

    m_playButton = new QPushButton(this);
    m_playButton->setText("Play");
    layout->addWidget(m_playButton);
    connect(m_playButton, SIGNAL(clicked()), m_media, SLOT(play()));

    m_volumeSlider = new Phonon::VolumeSlider(this);
    layout->addWidget(m_volumeSlider);
    m_volumeSlider->setAudioOutput(m_aoutput);

    setLayout(layout);
}

MediaPlayer::~MediaPlayer()
{
    delete m_aoutput;
}

void MediaPlayer::setDeviceName()
{
    Phonon::MediaSource mediaSource(Phonon::V4LVideo, m_deviceNameEdit->text());
    m_media->setCurrentSource(mediaSource);
    m_media->play();
}

