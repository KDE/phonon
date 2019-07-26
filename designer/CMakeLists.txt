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
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Designer
)

install(TARGETS phononwidgets DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
