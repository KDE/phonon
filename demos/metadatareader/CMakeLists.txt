cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(metadatareader)

set(CMAKE_AUTOMOC TRUE)

find_package(Phonon4Qt5 NO_MODULE) # Not required so it builds within phonon as well

set(metadatareader_SRCS
    main.cpp
    metadatareader.cpp
)

add_executable(metadatareader ${metadatareader_SRCS})
target_link_libraries(metadatareader Phonon::phonon4qt5 Qt5::Core Qt5::Widgets)
