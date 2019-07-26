project(metadatareader)
cmake_minimum_required(VERSION 2.6.9 FATAL_ERROR)

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(metadatareader_SRCS
    main.cpp
    metadatareader.cpp
)

phonon_add_executable(metadatareader ${metadatareader_SRCS})
target_link_libraries(metadatareader ${PHONON_LIBRARY} Qt5::Core Qt5::Widgets)
