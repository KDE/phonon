include_directories( ${KDE4_INCLUDES}  ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/videocapture/)

add_definitions( ${KDE4_DEFINITIONS} )

set(videocapturetestapp_SRCS videocapturetestappmain.cpp mainwindow.cpp )

kde4_add_ui_files(videocapturetestapp_SRCS mainwidget.ui )

kde4_add_executable(videocapturetestapp_bin ${videocapturetestapp_SRCS})

set_target_properties(videocapturetestapp_bin PROPERTIES OUTPUT_NAME videocapturetestapp)

target_link_libraries( videocapturetestapp_bin ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} videocapture)

install(TARGETS videocapturetestapp_bin DESTINATION ${BIN_INSTALL_DIR} )
install(FILES videocapturetestapp.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
kde4_install_icons(${ICON_INSTALL_DIR} )
