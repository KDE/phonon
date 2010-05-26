clear; moc phonon_test.h > moc_phonon_test.cxx; g++ -o phonon_test -Wall -g -lkdecore -lQtCore -lQtGui -lphonon moc_phonon_test.cxx phonon_test.cpp main.cpp 
