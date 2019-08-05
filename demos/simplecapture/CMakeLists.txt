cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simplecapture)

set(CMAKE_AUTOMOC TRUE)

find_package(Phonon4Qt5 NO_MODULE) # Not required so it builds within phonon as well

set(simplecapture_SRCS
    main.cpp
    capture.cpp
)

add_executable(simplecapture ${simplecapture_SRCS})
target_link_libraries(simplecapture Phonon::${PHONON_LIB_SONAME} Qt5::Core Qt5::Widgets)
