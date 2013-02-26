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

qt5_use_modules(phononwidgets Core Gui Widgets Designer)

target_link_libraries(phononwidgets
    ${PHONON_LIBS}
)

install(TARGETS phononwidgets DESTINATION ${PHONON_QT_PLUGIN_INSTALL_DIR})
