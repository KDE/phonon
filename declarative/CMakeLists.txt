include_directories(${CMAKE_CURRENT_BINARY_DIR}/../) # For phonon/ includes.

set(declarative_SRCS
    abstractmediaelement.cpp
    audiooutputelement.cpp
    plugin.cpp
    videoelement.cpp
)

phonon_add_declarative_plugin(phononqmlplugin ${declarative_SRCS})

target_link_libraries(phononqmlplugin
    ${PHONON_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    phonon
    phononexperimental
)

# TODO: (also remember the caching!)

install(TARGETS phononqmlplugin DESTINATION ${QT_IMPORTS_DIR}/Phonon)
