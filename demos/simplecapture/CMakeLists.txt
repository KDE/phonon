project(simplecapture)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(simplecapture_SRCS
    main.cpp
    capture.cpp
)

phonon_add_executable(simplecapture ${simplecapture_SRCS})

target_link_libraries(simplecapture ${QT_LIBRARIES} phonon)
