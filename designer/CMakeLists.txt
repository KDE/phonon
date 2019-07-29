set(phononwidgetsplugin_SRCS
    phononcollection.cpp
    seeksliderplugin.cpp
    videoplayerplugin.cpp
    videoplayertaskmenu.cpp
    videowidgetplugin.cpp
    volumesliderplugin.cpp
)
qt5_add_resources(phononwidgetsplugin_SRCS phononwidgets.qrc)

add_library(phononwidgets MODULE ${phononwidgetsplugin_SRCS})
target_link_libraries(phononwidgets
    Phonon::${PHONON_LIB_SONAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Designer
)

install(TARGETS phononwidgets DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
