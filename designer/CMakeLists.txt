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

install(TARGETS phononwidgets DESTINATION ${QT_PLUGINS_DIR}/designer)
