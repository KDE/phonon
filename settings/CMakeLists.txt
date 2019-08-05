add_definitions(-DPHONON_LIB_SONAME=\"${PHONON_LIB_SONAME}\")

set(SRCS
    backendselection.cpp
    backendselection.ui
    devicepreference.cpp
    devicepreference.ui
    main.cpp
    settings.cpp
    settings.qrc
    settings.ui
)

ecm_create_qm_loader(SRCS phononsettings_qt)

add_executable(phononsettings ${SRCS})
target_link_libraries(phononsettings
    Phonon::phonon4qt5
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

install(TARGETS phononsettings ${INSTALL_TARGETS_DEFAULT_ARGS})
