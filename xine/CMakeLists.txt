add_subdirectory(kcm)

include_directories(${XINE_INCLUDE_DIR})

set(phonon_xine_PART_SRCS
   sinknode.cpp
   sourcenode.cpp
   xinethread.cpp
   nullsink.cpp
    xineengine.cpp
    xinestream.cpp
    abstractaudiooutput.cpp
    audiodataoutput.cpp
    effect.cpp
    audiooutput.cpp
    mediaobject.cpp
   videodataoutput.cpp
    visualization.cpp
    backend.cpp
    volumefadereffect.cpp
    bytestream.cpp
    bytestreamplugin.cpp
    net_buf_ctrl.c
    volumefader_plugin.cpp
    plugins.c
   )

macro_optional_find_package(XCB)
SET(XCB_VIDEO TRUE)
if(XCB_FOUND AND XINE_XCB_FOUND)
  set(phonon_xine_PART_SRCS ${phonon_xine_PART_SRCS} videowidget.cpp)
  include_directories(${LIBXCB_INCLUDE_DIR})
  add_definitions(${LIBXCB_DEFINITIONS})
else(XCB_FOUND AND XINE_XCB_FOUND)
  add_definitions(-DPHONON_XINE_NO_VIDEOWIDGET)
  message(STATUS "Cannot compile VideoWidget for the xine backend. Needs xine version 1.1.5 or later and libxcb.")
  SET(XCB_VIDEO FALSE)
endif(XCB_FOUND AND XINE_XCB_FOUND)
macro_log_feature(XCB_VIDEO "XCB" "XCB is needed for the video widget, libxine needs to be compiled with XCB" "http://xcb.freedesktop.org/" REQUIRED)

macro_bool_to_01(XINEWIDGET_FOUND XCB_VIDEO)
configure_file(config-xine-widget.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xine-widget.h )

add_definitions(-DPHONON_BACKEND_VERSION_4_2)
kde4_add_plugin(phonon_xine ${phonon_xine_PART_SRCS})
target_link_libraries(phonon_xine ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_PHONON_LIBS} ${XINE_LIBRARY})
if(XCB_FOUND AND XINE_XCB_FOUND)
  target_link_libraries(phonon_xine ${LIBXCB_LIBRARIES})
endif(XCB_FOUND AND XINE_XCB_FOUND)

install(TARGETS phonon_xine DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES xine.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
kde4_install_icons(${ICON_INSTALL_DIR})
