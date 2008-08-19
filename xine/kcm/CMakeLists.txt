set(kcmphononxine_SRCS xineoptions.cpp)
kde4_add_ui_files(kcmphononxine_SRCS xineoptions.ui)

include_directories(${XINE_INCLUDE_DIR})

kde4_add_plugin(kcm_phononxine ${kcmphononxine_SRCS})
target_link_libraries(kcm_phononxine ${KDE4_KDEUI_LIBS} ${XINE_LIBRARY})
install(TARGETS kcm_phononxine DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_phononxine.desktop DESTINATION ${SERVICES_INSTALL_DIR})
