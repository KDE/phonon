include_directories(${CMAKE_CURRENT_BINARY_DIR}/../) # For phonon/ includes.

set(phonon_fake_SRCS
    audiooutput.cpp
    backend.cpp
    mediaobject.cpp
    pipeline.cpp
)

automoc4_add_library(phonon_fake MODULE ${phonon_fake_SRCS})

set_target_properties(phonon_fake PROPERTIES PREFIX "")
target_link_libraries(phonon_fake
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    phonon
)

install(TARGETS phonon_fake DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
