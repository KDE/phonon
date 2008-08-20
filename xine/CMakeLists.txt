#add_subdirectory(kcm)

include_directories(${XINE_INCLUDE_DIR})

set(phonon_xine_SRCS
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
  set(phonon_xine_SRCS ${phonon_xine_SRCS} videowidget.cpp)
  include_directories(${LIBXCB_INCLUDE_DIR})
  add_definitions(${LIBXCB_DEFINITIONS})
else(XCB_FOUND AND XINE_XCB_FOUND)
  add_definitions(-DPHONON_XINE_NO_VIDEOWIDGET)
  message(STATUS "Cannot compile VideoWidget for the xine backend. Needs xine version 1.1.5 or later and libxcb.")
  SET(XCB_VIDEO FALSE)
endif(XCB_FOUND AND XINE_XCB_FOUND)
macro_log_feature(XCB_VIDEO "XCB" "XCB is needed for the video widget, libxine needs to be compiled with XCB" "http://xcb.freedesktop.org/" REQUIRED)

SET(XINEWIDGET_FOUND TRUE)
if(XCB_FOUND)
else(XCB_FOUND)
   SET(XINEWIDGET_FOUND FALSE)
endif(XCB_FOUND)
configure_file(config-xine-widget.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xine-widget.h )

add_definitions(-DPHONON_BACKEND_VERSION_4_2)
automoc4(phonon_xine phonon_xine_SRCS)
add_library(phonon_xine SHARED ${phonon_xine_SRCS})
set_target_properties(phonon_xine PROPERTIES PREFIX "")
target_link_libraries(phonon_xine 
	${QT_QTCORE_LIBRARY} 
	${QT_QTGUI_LIBRARY}
	${PHONON_LIBS}
	${XINE_LIBRARY})
if(XCB_FOUND AND XINE_XCB_FOUND)
  target_link_libraries(phonon_xine ${LIBXCB_LIBRARIES})
endif(XCB_FOUND AND XINE_XCB_FOUND)

install(TARGETS phonon_xine DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES xine.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
