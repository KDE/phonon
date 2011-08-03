# Not finished yet
TEMPLATE = lib
CONFIG += dll qt debug warn_on
QT -= gui
VERSION = 1.0.0
HEADERS += davros.h block.h
SOURCES += davros.cpp \
           block.cpp

headers.files = $$HEADERS

unix {
    headers.path = DESTDIR/include
}   

INSTALLS += headers
