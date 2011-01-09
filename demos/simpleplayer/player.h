#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>
#include <QtCore/QUrl>
#include <phonon/phononnamespace.h>

class QPushButton;
namespace Phonon {
    class MediaObject;
}

class Player : public QWidget {

Q_OBJECT

public:
    Player(QWidget* parent = 0, Qt::WindowFlags f = 0);

public slots:
    void load(const QUrl &url);
    void play();
    void pause();

private slots:
    void mediaStateChange(Phonon::State newState, Phonon::State oldState);
    void playPause();

private:
    Phonon::MediaObject* m_media;
    QPushButton* m_playPause;
    QPushButton* m_stop;

};

#endif //PLAYER_H
