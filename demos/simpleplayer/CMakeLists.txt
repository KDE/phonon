project(simpleplayer)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS})
include_directories(${PHONON_INCLUDES} ${QT_INCLUDES})

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

qt4_automoc(${simpleplayer_SRCS})

add_executable(simpleplayer ${simpleplayer_SRCS} ${simpleplayer_MOC_SRCS})

target_link_libraries(simpleplayer ${QT_LIBRARIES} ${PHONON_LIBS})
