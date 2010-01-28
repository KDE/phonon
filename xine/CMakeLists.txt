#add_subdirectory(kcm)

# known limitations:
#  xinelib 1.1.1 adds audio
#  xinelib 1.1.2 adds audio with gapless playback
#  xinelib 1.1.5 + xcb adds video
#  xinelib 1.1.7 does not abort() when unplugging a USB device
#  xinelib 1.1.9 fixes playback of short sounds
#  xinelib 1.1.15 - xine headers broken - need patch to compiles
#  xinelib 1.2 untested => unsupported

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

macro_optional_find_package(Xine)
macro_optional_find_package(XCB)

if(XINE_FOUND)
    string(REGEX REPLACE "^[0-9]*\\.([0-9]*)\\.[0-9]*$" "\\1" XINE_MINOR_VERSION ${XINE_VERSION})
    string(REGEX REPLACE "^[0-9]*\\.[0-9]*\\." "" XINE_BUGFIX_VERSION ${XINE_VERSION})
    # if an older xinelib is installed give a warning about problems but continue
    if(XINE_MINOR_VERSION LESS 2 AND XINE_BUGFIX_VERSION LESS 90)
        if(XINE_BUGFIX_VERSION LESS 7)
            if(XINE_BUGFIX_VERSION LESS 5)
                macro_log_feature(TRUE "Xine" "xine-lib v${XINE_VERSION} was found on your system. This version does not provide video support and has known security issues. Recommend upgrading to version 1.1.9 or above." "http://sourceforge.net/project/showfiles.php?group_id=9655&package_id=9732")
            else(XINE_BUGFIX_VERSION LESS 5)
                macro_log_feature(TRUE "Xine" "xine-lib v${XINE_VERSION} was found on your system. This version is known to have problems when a device is unplugged. Consider upgrading to version 1.1.9 or above." "http://sourceforge.net/project/showfiles.php?group_id=9655&package_id=9732")
            endif(XINE_BUGFIX_VERSION LESS 5)
        else(XINE_BUGFIX_VERSION LESS 7)
            if(XINE_BUGFIX_VERSION LESS 9)
                macro_log_feature(TRUE "Xine" "xine-lib v${XINE_VERSION} was found on your system. This version is known to have problems when playing short sounds. Consider upgrading to version 1.1.9 or above." "http://sourceforge.net/project/showfiles.php?group_id=9655&package_id=9732")
            endif(XINE_BUGFIX_VERSION LESS 9)
        endif(XINE_BUGFIX_VERSION LESS 7)
    else(XINE_MINOR_VERSION LESS 2 AND XINE_BUGFIX_VERSION LESS 90)
        macro_log_feature(FALSE "Xine" "xine-lib v${XINE_VERSION} was found on your system. This version contains major changes compared to 1.1.x and has not been tested properly for use with this backend. Consider downgrading to version 1.1.9." "http://sourceforge.net/project/showfiles.php?group_id=9655&package_id=9732")
        set(XINE_FOUND FALSE)
    endif(XINE_MINOR_VERSION LESS 2 AND XINE_BUGFIX_VERSION LESS 90)
endif(XINE_FOUND)

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
    kequalizer_plugin.cpp
    plugins.c
    demux_wav.c
   )

SET(XCB_VIDEO TRUE)
if(XCB_FOUND AND XINE_XCB_FOUND)
  set(phonon_xine_SRCS ${phonon_xine_SRCS} videowidget.cpp)
  include_directories(${LIBXCB_INCLUDE_DIR})
  add_definitions(${LIBXCB_DEFINITIONS})
else(XCB_FOUND AND XINE_XCB_FOUND)
  add_definitions(-DPHONON_XINE_NO_VIDEOWIDGET)
  message(STATUS "Cannot compile VideoWidget for the xine backend. Needs xine version 1.1.9 or later and libxcb.")
  SET(XCB_VIDEO FALSE)
endif(XCB_FOUND AND XINE_XCB_FOUND)
macro_log_feature(XCB_VIDEO "XCB" "XCB is needed for the video widget, libxine needs to be compiled with XCB" "http://xcb.freedesktop.org/")

SET(XINEWIDGET_FOUND TRUE)
if(XCB_FOUND)
else(XCB_FOUND)
   SET(XINEWIDGET_FOUND FALSE)
endif(XCB_FOUND)



if(XINE_FOUND)
    include_directories(${XINE_INCLUDE_DIR})

    configure_file(config-xine-widget.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-xine-widget.h )
    
    add_definitions(-DPHONON_BACKEND_VERSION_4_4)
    automoc4_add_library(phonon_xine MODULE ${phonon_xine_SRCS})
    set_target_properties(phonon_xine PROPERTIES PREFIX "")
    target_link_libraries(phonon_xine ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${PHONON_LIBS} ${XINE_LIBRARY})
    if(XCB_FOUND AND XINE_XCB_FOUND)
        target_link_libraries(phonon_xine ${LIBXCB_LIBRARIES})
    endif(XCB_FOUND AND XINE_XCB_FOUND)

    install(TARGETS phonon_xine DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/phonon_backend)
    install(FILES xine.desktop DESTINATION ${SERVICES_INSTALL_DIR}/phononbackends)
    phonon_install_icons(${ICON_INSTALL_DIR})
else(XINE_FOUND)
    macro_log_feature(XINE_FOUND "Xine" "libxine is needed to compile phonon xine backend" "http://www.xine-project.org/")
endif(XINE_FOUND)
