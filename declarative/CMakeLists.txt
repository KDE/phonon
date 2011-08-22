include_directories(${CMAKE_CURRENT_BINARY_DIR}/../) # For phonon/ includes.

add_subdirectory(qml)

set(declarative_SRCS
    audiooutputelement.cpp
    mediaelement.cpp
    metadata.cpp
    plugin.cpp
    subtitleelement.cpp
    volumefadereffectelement.cpp
    videooutputelement.cpp

    # Work around bug in QtCreator not showing important things
    abstractinitable.h
    ${phonon_QMLS}
)

phonon_add_declarative_plugin(phononqmlplugin ${declarative_SRCS})

target_link_libraries(phononqmlplugin
    ${PHONON_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    phonon
)

install(TARGETS phononqmlplugin DESTINATION ${PHONON_QT_IMPORTS_INSTALL_DIR}/Phonon)
install(FILES ${phonon_QMLS} DESTINATION ${PHONON_QT_IMPORTS_INSTALL_DIR}/Phonon)
