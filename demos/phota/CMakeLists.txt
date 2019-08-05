cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(phota)

set(CMAKE_AUTOMOC TRUE)

find_package(Phonon4Qt5 NO_MODULE) # Not required so it builds within phonon as well

set(phota_SRCS
    main.cpp
    mainwindow.cpp
    mediacontroller.cpp
    videoeffects.cpp
)

qt5_wrap_ui(phota_SRCS videoeffects.ui)

add_executable(phota ${phota_SRCS})
target_link_libraries(phota Phonon::${PHONON_LIB_SONAME} Qt5::Core Qt5::Widgets)
