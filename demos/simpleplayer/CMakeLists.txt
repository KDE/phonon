cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simpleplayer)

set(CMAKE_AUTOMOC TRUE)

find_package(Phonon4Qt5 REQUIRED)

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

add_executable(simpleplayer ${simpleplayer_SRCS})
target_link_libraries(simpleplayer Phonon::${PHONON_LIB_SONAME} Qt5::Core Qt5::Widgets)
