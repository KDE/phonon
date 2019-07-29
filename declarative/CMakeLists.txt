add_subdirectory(qml)

set(declarative_SRCS
    audiooutputelement.cpp
    mediaelement.cpp
    metadata.cpp
    plugin.cpp
    subtitleelement.cpp
    volumefadereffectelement.cpp
    videoformatspy.cpp
    videooutputelement.cpp

    # Work around bug in QtCreator not showing important things
    abstractinitable.h
    ${phonon_QMLS}
)

add_library(phononqmlplugin MODULE ${declarative_SRCS})

target_link_libraries(phononqmlplugin
    Phonon::${PHONON_LIB_SONAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Declarative
)

install(TARGETS phononqmlplugin DESTINATION ${KDE_INSTALL_QTQUICKIMPORTSDIR}/Phonon)
install(FILES ${phonon_QMLS} DESTINATION ${KDE_INSTALL_QTQUICKIMPORTSDIR}/Phonon)
