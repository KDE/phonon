project(metadatareader)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 COMPONENTS QtCore REQUIRED)
include(${QT_USE_FILE})

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(metadatareader_SRCS
    main.cpp
    metadatareader.cpp
)

phonon_add_executable(metadatareader ${metadatareader_SRCS})

target_link_libraries(metadatareader ${QT_LIBRARIES} phonon)
