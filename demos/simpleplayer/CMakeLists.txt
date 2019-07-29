cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simpleplayer)

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

add_executable(simpleplayer ${simpleplayer_SRCS})
target_link_libraries(simpleplayer ${PHONON_LIBRARY} Qt5::Core Qt5::Widgets)
