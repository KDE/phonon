cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simplecapture)

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(simplecapture_SRCS
    main.cpp
    capture.cpp
)

add_executable(simplecapture ${simplecapture_SRCS})
target_link_libraries(simplecapture ${PHONON_LIBRARY} Qt5::Core Qt5::Widgets)
