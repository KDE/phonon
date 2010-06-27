
#include "capture_test.h"

MediaPlayer::MediaPlayer(QWidget *parent)
: QWidget(parent)
{
    m_deviceModel = NULL;

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_vwidget = new Phonon::VideoWidget(this);
    m_vwidget->setMinimumSize(QSize(400, 300));
    m_vwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_vwidget);

    m_aoutput = new Phonon::AudioOutput();

    m_media = new Phonon::MediaObject();

    Phonon::createPath(m_media, m_aoutput);
    Phonon::createPath(m_media, m_vwidget);

    QHBoxLayout *deviceNameLayout = new QHBoxLayout(this);

    m_deviceNameCombo = new QComboBox(this);
    m_deviceNameCombo->setEditable(false);
    deviceNameLayout->addWidget(m_deviceNameCombo);
    connect(m_deviceNameCombo, SIGNAL(activated(int)), this, SLOT(setDeviceIndex(int)));
    updateDeviceList();

    layout->addItem(deviceNameLayout);

    m_playButton = new QPushButton(this);
    m_playButton->setText("Play");
    connect(m_playButton, SIGNAL(clicked()), m_media, SLOT(play()));

    m_pauseButton = new QPushButton(this);
    m_pauseButton->setText("Pause");
    connect(m_pauseButton, SIGNAL(clicked()), m_media, SLOT(pause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText("Stop");
    connect(m_stopButton, SIGNAL(clicked()), m_media, SLOT(stop()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_pauseButton);
    buttonsLayout->addWidget(m_stopButton);
    layout->addItem(buttonsLayout);

    m_volumeSlider = new Phonon::VolumeSlider(this);
    layout->addWidget(m_volumeSlider);
    m_volumeSlider->setAudioOutput(m_aoutput);

    setLayout(layout);
}

MediaPlayer::~MediaPlayer()
{
    delete m_aoutput;
}

void MediaPlayer::setDeviceIndex(int index)
{
    #ifndef QT_NO_PHONON_VIDEOCAPTURE
    QModelIndex mi = m_deviceModel->index(index, 0, QModelIndex());
    Phonon::VideoCaptureDevice vc = m_deviceModel->modelData(mi);
    Phonon::MediaSource mediaSource(Phonon::AudioCaptureDevice(), vc);
    m_media->setCurrentSource(mediaSource);
    m_media->play();
    #endif
}

void MediaPlayer::updateDeviceList()
{
    #ifndef QT_NO_PHONON_VIDEOCAPTURE
    QList<Phonon::VideoCaptureDevice> l = Phonon::BackendCapabilities::availableVideoCaptureDevices();

    if (!m_deviceModel)
        m_deviceModel = new Phonon::VideoCaptureDeviceModel(l, 0);
    m_deviceNameCombo->setModel(m_deviceModel);
    #endif
}

