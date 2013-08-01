project(phota)
cmake_minimum_required(VERSION 2.6.9 FATAL_ERROR)

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

phonon_add_executable(phota ${phota_SRCS})

qt5_use_modules(phota Core Widgets)
target_link_libraries(phota ${PHONON_LIBRARY})
