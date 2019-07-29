cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(phota)

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(phota_SRCS
    main.cpp
    mainwindow.cpp
    mediacontroller.cpp
    videoeffects.cpp
)

qt5_wrap_ui(phota_SRCS videoeffects.ui)

add_executable(phota ${phota_SRCS})
target_link_libraries(phota ${PHONON_LIBRARY} Qt5::Core Qt5::Widgets)
