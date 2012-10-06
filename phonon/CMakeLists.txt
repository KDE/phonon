macro_optional_find_package(OpenGL)
macro_log_feature(OPENGL_FOUND "OpenGL" "A cross-platform graphics programming language." "http://www.opengl.org" FALSE)
macro_log_feature(QT_QTOPENGL_FOUND "QtOpenGL" "Qt Bindngs for OpenGL." "http://www.qt-project.org" FALSE)
if (OPENGL_FOUND AND QT_QTOPENGL_FOUND)
    set(PHONON_OPENGLSUPPORT TRUE CACHE BOOL "Has Phonon OpenGL support ?")
    if(PHONON_OPENGLSUPPORT)
        add_definitions(-DHAVE_OPENGL)
    endif(PHONON_OPENGLSUPPORT)
else (OPENGL_FOUND AND QT_QTOPENGL_FOUND)
    set(PHONON_OPENGLSUPPORT FALSE CACHE BOOL "Has Phonon OpenGL support ?")
endif (OPENGL_FOUND AND QT_QTOPENGL_FOUND)

macro_optional_find_package(PulseAudio 0.9.15)
macro_log_feature(PULSEAUDIO_FOUND "PulseAudio" "A cross-platform, networked sound server." "http://www.pulseaudio.org" FALSE "" "Allows audio playback via the PulseAudio soundserver when it is running")
macro_optional_find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "GLib2" "GLib 2 is required to compile the pulseaudio for Phonon" "http://www.gtk.org/download/" FALSE)

if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT TRUE CACHE BOOL "Has Phonon pulseaudio support ?")
   if(PHONON_PULSESUPPORT)
       add_definitions(-DHAVE_PULSEAUDIO)
       include_directories(${GLIB2_INCLUDE_DIR} ${PULSEAUDIO_INCLUDE_DIR})
   endif(PHONON_PULSESUPPORT)
else(GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT FALSE CACHE BOOL "Has Phonon pulseaudio support ?")
endif(GLIB2_FOUND AND PULSEAUDIO_FOUND)

macro_optional_find_package(QZeitgeist 0.8)
macro_log_feature(QZEITGEIST_FOUND "QZeitgeist" "Qt bindings for Zeitgeist" "http://projects.kde.org/kdesupport/libqzeitgeist/" FALSE "0.8")
if (QZEITGEIST_FOUND)
    add_definitions(-DHAVE_QZEITGEIST)
    include_directories(${QZEITGEIST_INCLUDE_DIR})
endif(QZEITGEIST_FOUND)

include_directories(${CMAKE_BINARY_DIR}/includes/phonon)

if (PHONON_BUILD_EXPERIMENTAL)
    add_subdirectory(experimental)
endif (PHONON_BUILD_EXPERIMENTAL)

set(phonon_LIB_SRCS
   abstractaudiooutput.cpp
   abstractaudiooutput_p.cpp
   abstractmediastream.cpp
   abstractvideooutput.cpp
   abstractvideooutput_p.cpp
   audiodataoutput.cpp
   audiooutput.cpp
   audiooutputinterface.cpp
   backendcapabilities.cpp
   effect.cpp
   effectparameter.cpp
   effectwidget.cpp
   factory.cpp
   globalconfig.cpp
   iodevicestream.cpp
   mediacontroller.cpp
   mediaobject.cpp
   medianode.cpp
   mediasource.cpp
   mrl.cpp
   objectdescription.cpp
   objectdescriptionmodel.cpp
   path.cpp
   phononnamespace.cpp
   platform.cpp
   pulsesupport.cpp
   seekslider.cpp
   statesvalidator.cpp
   streaminterface.cpp
   swiftslider.cpp
   volumefadereffect.cpp
   volumeslider.cpp
   videoplayer.cpp
   videowidget.cpp
   graphicsview/qpainterpainter.cpp
   graphicsview/videographicsobject.cpp
   )

if (PHONON_OPENGLSUPPORT)
   list(APPEND phonon_LIB_SRCS
      graphicsview/glpainter.cpp
      graphicsview/glslpainter.cpp
   )
endif (PHONON_OPENGLSUPPORT)

if (PHONON_PULSESUPPORT)
   list(APPEND phonon_LIB_SRCS
      pulsestream.cpp
   )
endif (PHONON_PULSESUPPORT)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
   list(APPEND phonon_LIB_SRCS
      audiooutputadaptor.cpp
   )
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

option(PHONON_NO_PLATFORMPLUGIN "Do not use any platform plugin")
if (PHONON_NO_PLATFORMPLUGIN)
    add_definitions(-DQT_NO_PHONON_PLATFORMPLUGIN)
endif (PHONON_NO_PLATFORMPLUGIN)

add_definitions(-DPHONON_LIBRARY_PATH="${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR}/plugins")

automoc4_add_library(phonon SHARED ${phonon_LIB_SRCS})
target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

if (PHONON_PULSESUPPORT)
    target_link_libraries(phonon ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
endif (PHONON_PULSESUPPORT)

if (PHONON_OPENGLSUPPORT)
    target_link_libraries(phonon ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY})
endif (PHONON_OPENGLSUPPORT)

if (QZEITGEIST_FOUND)
    target_link_libraries(phonon ${QZEITGEIST_LIBRARY})
endif(QZEITGEIST_FOUND)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
  target_link_libraries(phonon ${QT_QTDBUS_LIBRARY})
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  # We need to explicitly link libm to phonon in Solaris
  target_link_libraries(phonon m)
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

set_target_properties(phonon PROPERTIES
                             VERSION ${PHONON_LIB_VERSION}
                             SOVERSION ${PHONON_LIB_SOVERSION}
                             DEFINE_SYMBOL MAKE_PHONON_LIB
                             LINK_INTERFACE_LIBRARIES ""
                             )
# LINK_INTERFACE_LIBRARIES: By default don't add any linked libraries to the "exported"
# link interfaces, so that executables linking against this library
# will not automatically add implicit dependencies to their link list.
# This reduces inter-package dependencies and makes it easier to remove
# dependencies of shared libraries without breaking binary compatibility.



install(TARGETS phonon EXPORT PhononLibs ${INSTALL_TARGETS_DEFAULT_ARGS} )
# We write these to a strange place so the demos can find them properly
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononconfig_p.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononconfig_p.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h)
# Required for demos/cmake/FindPhonon.cmake
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/../includes/phonon/phononnamespace.h)

install(FILES
   abstractaudiooutput.h
   abstractmediastream.h
   abstractvideooutput.h
   addoninterface.h
   audiodataoutput.h
   audiodataoutputinterface.h
   audiooutput.h
   audiooutputinterface.h
   backendcapabilities.h
   backendinterface.h
   effect.h
   effectinterface.h
   effectparameter.h
   effectwidget.h
   globalconfig.h
   globaldescriptioncontainer.h
   mediacontroller.h
   medianode.h
   mediaobject.h
   mediaobjectinterface.h
   mediasource.h
   mrl.h
   objectdescription.h
   objectdescriptionmodel.h
   path.h
   phonon_export.h
   phonondefs.h
   platformplugin.h
   pulsesupport.h
   seekslider.h
   streaminterface.h
   videoplayer.h
   videowidget.h
   videowidgetinterface.h
   volumefadereffect.h
   volumefaderinterface.h
   volumeslider.h
   graphicsview/videoframe.h
   graphicsview/videographicsobjectinterface.h
   ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
    install(FILES org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR})
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

