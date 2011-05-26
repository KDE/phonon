include_directories(${CMAKE_CURRENT_BINARY_DIR}/../) # For phonon/ includes.

set(declarative_SRCS
    plugin.cpp
)

phonon_add_declarative_plugin(PhononDeclarativePlugin ${declarative_SRCS})

target_link_libraries(PhononDeclarativePlugin
    ${PHONON_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    phonon
)

# TODO: install (also remember the caching!)
