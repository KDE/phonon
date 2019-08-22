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

# Techincally since 5.9 we should only need uiplugins and not designer,
# in the interest of simplicity we still look for and use the designer
# target though so things definitely build on <5.9

# To bypass warnings about using the wrong/deprecated headers make
# sure to prefer UiPlugins whenever available.
if(Qt5UiPlugin_VERSION AND NOT Qt5UiPlugin_VERSION VERSION_LESS "5.9.0")
    target_link_libraries(phononwidgets Qt5::UiPlugin)
else()
    # In <5.9 one would ordinarily include QtDesigner/Foo to
    # get unified code we'll manually inject the include dir
    # into the target so we get away with Foo.
    target_include_directories(phononwidgets
        PRIVATE ${Qt5UiPlugin_INCLUDE_DIRS}
        PRIVATE ${Qt5Designer_INCLUDE_DIRS}
    )
endif()

target_link_libraries(phononwidgets
    Phonon::${PHONON_LIB_SONAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Designer
)

install(TARGETS phononwidgets DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/designer)
