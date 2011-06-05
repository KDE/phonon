include_directories(${CMAKE_CURRENT_BINARY_DIR}/../) # For phonon/ includes.

set(phononwidgetsplugin_SRCS
    phononcollection.cpp
    seeksliderplugin.cpp
    videoplayerplugin.cpp
    videoplayertaskmenu.cpp
    videowidgetplugin.cpp
    volumesliderplugin.cpp
)

phonon_add_designer_plugin(phononwidgets phononwidgets.qrc ${phononwidgetsplugin_SRCS})

target_link_libraries(phononwidgets
    ${PHONON_LIBS}
    ${QT_QTDESIGNER_LIBRARY}
    phonon
)


set(PHONON_QT_PLUGIN_INSTALL_DIR ${QT_PLUGINS_DIR}/designer CACHE PATH "The directory where Phonon Qt plugins will be installed to.")


install(TARGETS phononwidgets DESTINATION ${PHONON_QT_PLUGIN_INSTALL_DIR})
