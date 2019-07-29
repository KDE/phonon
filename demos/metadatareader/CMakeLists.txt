cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(metadatareader)

find_package(Phonon REQUIRED)

set(metadatareader_SRCS
    main.cpp
    metadatareader.cpp
)

add_executable(metadatareader ${metadatareader_SRCS})
target_link_libraries(metadatareader Phonon::${PHONON_LIB_SONAME} Qt5::Core Qt5::Widgets)
