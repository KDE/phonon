if (PHONON_BUILD_EXPERIMENTAL)
    add_definitions(-DPHONON_EXPERIMENTAL)
    add_subdirectory(experimental)
endif (PHONON_BUILD_EXPERIMENTAL)

macro_optional_find_package(PulseAudio 0.9.15)
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

macro_optional_find_package(QZeitgeist 0.8)
macro_log_feature(QZEITGEIST_FOUND "QZeitgeist" "Qt bindings for Zeitgeist" "http://projects.kde.org/kdesupport/libqzeitgeist/" FALSE "0.8")
if (QZEITGEIST_FOUND)
    add_definitions(-DHAVE_QZEITGEIST)
    include_directories(${QZEITGEIST_INCLUDE_DIR})
endif(QZEITGEIST_FOUND)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} audio effects input video widgets)
set(phonon_LIB_SRCS
   audio/abstractaudiooutput.cpp
   audio/abstractaudiooutput_p.cpp
   audio/audiodataoutput.cpp
   audio/audiooutput.cpp
   audio/audiooutputinterface.cpp
   audio/pulsesupport.cpp

   effects/effect.cpp
   effects/effectparameter.cpp
   effects/volumefadereffect.cpp

   input/abstractmediastream.cpp
   input/iodevicestream.cpp
   input/streaminterface.cpp

   video/abstractvideooutput.cpp
   video/abstractvideooutput_p.cpp

   widgets/effectwidget.cpp
   widgets/seekslider.cpp
   widgets/swiftslider.cpp
   widgets/volumeslider.cpp
   widgets/videoplayer.cpp
   widgets/videowidget.cpp

   backendcapabilities.cpp
   factory.cpp
   globalconfig.cpp
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
)

if (PULSEAUDIO_FOUND)
   list(APPEND phonon_LIB_SRCS
      audio/pulsestream.cpp
   )
endif (PULSEAUDIO_FOUND)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
   list(APPEND phonon_LIB_SRCS
      audio/audiooutputadaptor.cpp
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
    target_link_libraries(phonon ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
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
   audio/abstractaudiooutput.h
   audio/audiodataoutput.h
   audio/audiodataoutputinterface.h
   audio/audiooutput.h
   audio/audiooutputinterface.h
   audio/pulsesupport.h

   effects/effect.h
   effects/effectinterface.h
   effects/effectparameter.h
   effects/volumefadereffect.h
   effects/volumefaderinterface.h

   input/abstractmediastream.h
   input/streaminterface.h

   video/abstractvideooutput.h

   widgets/effectwidget.h
   widgets/seekslider.h
   widgets/videoplayer.h
   widgets/videowidget.h
   widgets/videowidgetinterface.h
   widgets/volumeslider.h

   addoninterface.h
   backendcapabilities.h
   backendinterface.h
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
   ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel
)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
    install(FILES audio/org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR})
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

