cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(simpleplayer)

find_package(Phonon REQUIRED)

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

add_executable(simpleplayer ${simpleplayer_SRCS})
target_link_libraries(simpleplayer Phonon::${PHONON_LIB_SONAME} Qt5::Core Qt5::Widgets)
