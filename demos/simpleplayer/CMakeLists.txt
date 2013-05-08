project(simpleplayer)
cmake_minimum_required(VERSION 2.6.9 FATAL_ERROR)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(Phonon5 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

add_executable(simpleplayer ${simpleplayer_SRCS})

qt5_use_modules(simpleplayer Core Widgets)
target_link_libraries(simpleplayer ${PHONON_LIBRARY})
