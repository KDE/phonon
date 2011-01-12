#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QWidget>
#include <QtCore/QUrl>
#include <phonon/Global>

class QPushButton;
namespace Phonon {
    class MediaObject;
}

/**
 * @brief A simple media player widget
 */
class Player : public QWidget {

Q_OBJECT

public:
    Player(QWidget* parent = 0, Qt::WindowFlags f = 0);

public slots:

    /**
     * @brief Load a specific URL
     */
    void load(const QUrl &url);

    /**
     * @brief Prompt the user for a file to load
     */
    void load();

private slots:

    /**
     * @brief Updates the GUI when the underlying MediaObject changes states
     */
    void mediaStateChanged(Phonon::State newState, Phonon::State oldState);

    /**
     * @brief Plays or pauses the media, depending on current state
     */
    void playPause();

private:
    Phonon::MediaObject* m_media;
    QPushButton* m_playPause;
    QPushButton* m_stop;

};

#endif //PLAYER_H
