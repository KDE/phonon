#include "phonon_test.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName("Primitive Phonon Application");
  
  MediaPlayer mp(NULL);
  mp.setWindowTitle("Primitive Phonon Application");
  mp.show();
  
  return app.exec();
}
