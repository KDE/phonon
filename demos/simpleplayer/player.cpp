#include "player.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/VideoWidget>
#include <phonon/SeekSlider>

Player::Player(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    m_media = new Phonon::MediaObject(this);
    //The mediaStateChanged slot will update the GUI elements to reflect what
    //the user can do next
    connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State, Phonon::State)));

    Phonon::AudioOutput* audioOut = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::VideoWidget* videoOut = new Phonon::VideoWidget(this);
    videoOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //By default, there is no minimum size on a video widget. Once a video
    //stream is available, making the widget visible won't actually show the
    //video, since it'll still be a 0x0 widget.
    videoOut->setMinimumSize(100, 100);

    //After a MediaSource is loaded, this signal will be emitted to let us know
    //if a video stream was found.
    connect(m_media, SIGNAL(hasVideoChanged(bool)), videoOut, SLOT(setVisible(bool)));

    //Link the media object to our audio and video outputs.
    Phonon::createPath(m_media, audioOut);
    Phonon::createPath(m_media, videoOut);

    //This widget will contain the stop/pause buttons
    QWidget* buttonBar = new QWidget(this);

    m_playPause = new QPushButton(tr("Play"), buttonBar);
    m_stop = new QPushButton(tr("Stop"), buttonBar);

    Phonon::SeekSlider *seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setMediaObject(m_media);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(videoOut);
    layout->addWidget(seekSlider);
    layout->addWidget(buttonBar);
    setLayout(layout);

    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->addWidget(m_stop);
    buttonLayout->addWidget(m_playPause);
    buttonBar->setLayout(buttonLayout);

    m_stop->setEnabled(false);

    connect(m_stop, SIGNAL(clicked(bool)), m_media, SLOT(stop()));
    connect(m_playPause, SIGNAL(clicked(bool)), this, SLOT(playPause()));

    videoOut->setVisible(false);
}

void Player::playPause()
{
    if (m_media->state() == Phonon::PlayingState) {
        m_media->pause();
    } else {
        if (m_media->currentSource().type() == Phonon::MediaSource::Empty)
            load();
        m_media->play();
    }
}

void Player::load(const QUrl &url)
{
    if (url.scheme().isEmpty())
        m_media->setCurrentSource(QUrl::fromLocalFile(url.toString()));
    else
        m_media->setCurrentSource(url);
    m_media->play();
}

void Player::load()
{
    QString url = QFileDialog::getOpenFileName(this);
    if (url.isEmpty())
        return;
    load(QUrl::fromLocalFile(url));
}

void Player::mediaStateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch(newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_playPause->setText(tr("Play"));
        m_stop->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playPause->setText(tr("Pause"));
        m_stop->setEnabled(true);
        break;
    case Phonon::BufferingState:
        break;
    case Phonon::PausedState:
        m_playPause->setText(tr("Play"));
        break;
    case Phonon::ErrorState:
        break;
    }
}

#include "player.moc"

