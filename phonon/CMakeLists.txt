if(NOT PHONON_NO_GRAPHICSVIEW)
    message(STATUS "Building graphicsview.")
    add_subdirectory(graphicsview)
else()
    message(STATUS "Not Building graphicsview.")
    add_definitions(-DPHONON_NO_GRAPHICSVIEW)
endif()

if (PHONON_BUILD_EXPERIMENTAL)
    add_subdirectory(experimental)
endif (PHONON_BUILD_EXPERIMENTAL)

configure_file(phononconfig_p.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononconfig_p.h @ONLY)

message(AUTHOR_WARNING "TODO: we configure for phonon_capture cmakedefine but its unclear why this needs to be in a public header")
configure_file(phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h @ONLY)
# We write these to a strange place so the demos can find them properly
message(AUTHOR_WARNING "TODO: use internal target and set include dirs on target")
configure_file(phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/../includes/phonon/phononnamespace.h @ONLY)

find_package(PulseAudio 0.9.15)
set_package_properties(PulseAudio PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "A cross-platform, networked sound server"
    URL "http://www.pulseaudio.org")
find_package(GLIB2)
set_package_properties(GLIB2 PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "GLib 2 is required to compile the pulseaudio for Phonon"
    URL "http://www.gtk.org/download/")

if (GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT TRUE CACHE BOOL "Has Phonon pulseaudio support ?")
   if(PHONON_PULSESUPPORT)
       add_definitions(-DHAVE_PULSEAUDIO)
   endif(PHONON_PULSESUPPORT)
   include_directories(${GLIB2_INCLUDE_DIR} ${PULSEAUDIO_INCLUDE_DIR})
else(GLIB2_FOUND AND PULSEAUDIO_FOUND)
   set(PHONON_PULSESUPPORT FALSE CACHE BOOL "Has Phonon pulseaudio support ?")
   set(PULSEAUDIO_INCLUDE_DIR "")
   set(PULSEAUDIO_LIBRARY "")
   set(PULSEAUDIO_MAINLOOP_LIBRARY "")
endif(GLIB2_FOUND AND PULSEAUDIO_FOUND)

include_directories(${CMAKE_BINARY_DIR}/includes/phonon)

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
   ${phonon_VGO_SRCS}
   )

ecm_create_qm_loader(phonon_LIB_SRCS libphonon_qt)

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
add_definitions(-DPHONON_BACKEND_DIR_SUFFIX="/${PHONON_LIB_SONAME}_backend/")

set(optional_link_libraries)

if(QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
    list(APPEND optional_link_libraries Qt5::DBus)
endif()

if (PHONON_PULSESUPPORT)
    list(APPEND optional_link_libraries ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
endif (PHONON_PULSESUPPORT)

if(NOT PHONON_NO_GRAPHICSVIEW)
    list(APPEND optional_link_libraries OpenGL Qt5::OpenGl)
endif()

add_library(${PHONON_LIB_SONAME} SHARED ${phonon_LIB_SRCS})
target_link_libraries(${PHONON_LIB_SONAME}
    PUBLIC
        Qt5::Core
        Qt5::Widgets
    PRIVATE
        ${optional_link_libraries}
)

set_target_properties(${PHONON_LIB_SONAME}
    PROPERTIES
        VERSION ${PHONON_VERSION}
        SOVERSION ${PHONON_SOVERSION}
        DEFINE_SYMBOL MAKE_PHONON_LIB)

target_include_directories(${PHONON_LIB_SONAME}
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>")

install(TARGETS ${PHONON_LIB_SONAME} EXPORT PhononLibs ${INSTALL_TARGETS_DEFAULT_ARGS} )

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
   ${phonon_VGO_HDRS}
   ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h
   ${CMAKE_CURRENT_BINARY_DIR}/phonon_version.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

if (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.Phonon.AudioOutput.xml ${CMAKE_CURRENT_BINARY_DIR}/org.kde.${PHONON_LIB_SONAME_CAMEL}.AudioOutput.xml @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.${PHONON_LIB_SONAME_CAMEL}.AudioOutput.xml DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR})
endif (QT_QTDBUS_FOUND AND NOT PHONON_NO_DBUS)

