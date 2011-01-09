#include <QtGui/QApplication>
#include <QtCore/QUrl>
#include "player.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    //On most platforms, this tells the audio system the name of the application responsible for which stream
    app.setApplicationName("simpleplayer");

    Player p;
    p.show();

    QStringList args = QApplication::arguments();
    if (args.size() > 1) {
        p.load(QUrl(args[1]));
        p.play();
    }

    return app.exec();
}
