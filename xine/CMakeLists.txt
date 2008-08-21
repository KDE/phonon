#add_subdirectory(kcm)

include_directories(${XINE_INCLUDE_DIR})

if (UNIX)
   check_cxx_source_compiles("
#include <sys/types.h>
 /* Check that off_t can represent 2**63 - 1 correctly.
    We can't simply define LARGE_OFF_T to be 9223372036854775807,
    since some C++ compilers masquerading as C compilers
    incorrectly reject 9223372036854775807.  */
#define LARGE_OFF_T (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))

  int off_t_is_large[(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1) ? 1 : -1];
  int main() { return 0; }
" _OFFT_IS_64BIT)

   if (NOT _OFFT_IS_64BIT)
     add_definitions(-D_FILE_OFFSET_BITS=64)
   endif (NOT _OFFT_IS_64BIT)
endif (UNIX)

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

install(TARGETS phonon_xine DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
install(FILES xine.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
