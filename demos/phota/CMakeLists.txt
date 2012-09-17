project(phota)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(phota_SRCS
    main.cpp
    mainwindow.cpp
    mediacontroller.cpp
    videoeffects.cpp
)

qt4_wrap_ui(phota_SRCS videoeffects.ui)

phonon_add_executable(phota ${phota_SRCS})

target_link_libraries(phota ${QT_LIBRARIES} phonon)
