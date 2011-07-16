set(PULSEAUDIO_FIND_VERSION "0.9.15")
macro_optional_find_package(PulseAudio)
macro_log_feature(PULSEAUDIO_FOUND "PulseAudio" "A cross-platform, networked sound server." "http://www.pulseaudio.org" FALSE "" "Allows audio playback via the PulseAudio soundserver when it is running")
macro_optional_find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "GLib2" "GLib 2 is required to compile the pulseaudio for Phonon" "http://www.gtk.org/download/" FALSE)


if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT TRUE CACHE BOOL "Has Phonon pulseaudio support ?" FORCE) # this one must be forced into the cache so it exists also in the toplevel CMakeLists.txt
   add_definitions(-DHAVE_PULSEAUDIO)
   include_directories(${GLIB2_INCLUDE_DIR} ${PULSEAUDIO_INCLUDE_DIR})
else(GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT FALSE CACHE BOOL "Has Phonon pulseaudio support ?" FORCE) # this one must be forced into the cache so it exists also in the toplevel CMakeLists.txt
   set(PULSEAUDIO_INCLUDE_DIR "")
   set(PULSEAUDIO_LIBRARY "")
   set(PULSEAUDIO_MAINLOOP_LIBRARY "")
endif(GLIB2_FOUND AND PULSEAUDIO_FOUND)

macro_optional_find_package(QZeitgeist NO_MODULE )
macro_log_feature(QZEITGEIST_FOUND "QtZeitgeist" "Qt bindings for Zeitgeist" "http://gitorious.org/kde-zeitgeist/libqzeitgeist/" FALSE)
if (QZEITGEIST_FOUND)
    add_definitions(-DHAVE_QZEITGEIST)
    include_directories(${QZEITGEIST_INCLUDE_DIR})
endif(QZEITGEIST_FOUND)

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
   streaminterface.cpp
   swiftslider.cpp
   volumefadereffect.cpp
   volumeslider.cpp
   videoplayer.cpp
   videowidget.cpp
   )

if (PULSEAUDIO_FOUND)
   list(APPEND phonon_LIB_SRCS
      pulsestream.cpp
   )
endif (PULSEAUDIO_FOUND)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
   list(APPEND phonon_LIB_SRCS
      audiooutputadaptor.cpp
   )
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

option(PHONON_NO_PLATFORMPLUGIN "Do not use any platform plugin")
if (PHONON_NO_PLATFORMPLUGIN)
    add_definitions(-DQT_NO_PHONON_PLATFORMPLUGIN)
endif (PHONON_NO_PLATFORMPLUGIN)

add_definitions(-DPHONON_LIBRARY_PATH="${PLUGIN_INSTALL_DIR}/plugins")

automoc4_add_library(phonon SHARED ${phonon_LIB_SRCS})
target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
    target_link_libraries(phonon ${GLIB2_LIBRARIES} ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
endif (GLIB2_FOUND AND PULSEAUDIO_FOUND)

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
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononconfig_p.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononconfig_p.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h)

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
   ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
    install(FILES org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR})
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

