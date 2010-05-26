
#include "phonon_test.h"

MediaPlayer::MediaPlayer(QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_vwidget = new Phonon::VideoWidget(this);
    m_vwidget->setMinimumSize(QSize(400, 300));
    layout->addWidget(m_vwidget);

    m_aoutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);

    m_media = new Phonon::MediaObject();

    Phonon::createPath(m_media, m_aoutput);
    Phonon::createPath(m_media, m_vwidget);

    QHBoxLayout *urlLayout = new QHBoxLayout(this);

    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setText("URL");
    urlLayout->addWidget(m_urlEdit);
    connect(m_urlEdit, SIGNAL(editingFinished()), this, SLOT(setUrl()));

    m_browseButton = new QPushButton(this);
    m_browseButton->setText("Browse...");
    urlLayout->addWidget(m_browseButton);
    connect(m_browseButton, SIGNAL(clicked()), this, SLOT(browseUrl()));

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

void MediaPlayer::setUrl()
{
    m_media->setCurrentSource(KUrl(m_urlEdit->text()));
    m_media->play();
}

void MediaPlayer::browseUrl()
{
    QString newUrl = QFileDialog::getOpenFileName(this, "Open Media File", "~");
    if (!newUrl.isEmpty())
    {
        m_urlEdit->setText(newUrl);
        setUrl();
    }
}

