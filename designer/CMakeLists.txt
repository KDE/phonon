set(phononwidgetsplugin_SRCS
    phononcollection.cpp
    seeksliderplugin.cpp
    videoplayerplugin.cpp
    videoplayertaskmenu.cpp
    videowidgetplugin.cpp
    volumesliderplugin.cpp
)
qt_add_resources(phononwidgetsplugin_SRCS phononwidgets.qrc)

add_library(phononwidgets MODULE ${phononwidgetsplugin_SRCS})

# Technically since 5.9 we should only need uiplugins and not designer,
# in the interest of simplicity we still look for and use the designer
# target though so things definitely build on <5.9

if(Qt5UiPlugin_VERSION)
    target_link_libraries(phononwidgets Qt${QT_MAJOR_VERSION}::UiPlugin)
endif()

target_link_libraries(phononwidgets
    Phonon::${PHONON_LIB_SONAME}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Designer
)

install(TARGETS phononwidgets DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/designer)
