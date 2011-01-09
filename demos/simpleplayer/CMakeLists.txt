project(simpleplayer)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(simpleplayer_SRCS
    main.cpp
    player.cpp
)

qt4_automoc(${simpleplayer_SRCS})

add_executable(simpleplayer ${simpleplayer_SRCS} ${simpleplayer_MOC_SRCS})

target_link_libraries(simpleplayer ${QT_LIBRARIES} phonon phononexperimental)
