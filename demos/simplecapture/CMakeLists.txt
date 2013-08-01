project(simplecapture)
cmake_minimum_required(VERSION 2.6.9 FATAL_ERROR)

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(simplecapture_SRCS
    main.cpp
    capture.cpp
)

phonon_add_executable(simplecapture ${simplecapture_SRCS})

qt5_use_modules(simplecapture Core Widgets)
target_link_libraries(simplecapture ${PHONON_LIBRARY})
