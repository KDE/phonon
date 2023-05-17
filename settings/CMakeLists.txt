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

if(${QT_MAJOR_VERSION} EQUAL 6)
  ecm_create_qm_loader(SRCS phononsettings6_qt)

  add_executable(phononsettings6 ${SRCS})
  target_link_libraries(phononsettings6
    Phonon::phonon4qt${QT_MAJOR_VERSION}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    )

  install(TARGETS phononsettings6 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
else()
  ecm_create_qm_loader(SRCS phononsettings_qt)

  add_executable(phononsettings ${SRCS})
  target_link_libraries(phononsettings
    Phonon::phonon4qt${QT_MAJOR_VERSION}
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    )

  install(TARGETS phononsettings ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
endif()
