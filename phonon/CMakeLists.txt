if(PHONON_BUILD_EXPERIMENTAL)
    add_subdirectory(experimental)
endif()

configure_file(phononconfig_p.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononconfig_p.h @ONLY)
configure_file(phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h @ONLY)

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

option(PHONON_NO_PLATFORMPLUGIN "Do not use any platform plugin")
if (PHONON_NO_PLATFORMPLUGIN)
    add_definitions(-DQT_NO_PHONON_PLATFORMPLUGIN)
endif (PHONON_NO_PLATFORMPLUGIN)

add_definitions(-DPHONON_PLUGIN_PATH="${PLUGIN_INSTALL_DIR}")
add_definitions(-DPHONON_BACKEND_DIR_SUFFIX="/${BACKEND_DIR_SUFFIX}/")

set(optional_link_libraries)

if(PHONON_PULSESUPPORT)
    list(APPEND phonon_LIB_SRCS pulsestream.cpp)
    list(APPEND optional_link_libraries ${GOBJECT_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})
endif()

add_library(${PHONON_LIB_SONAME} SHARED ${phonon_LIB_SRCS})
add_library(Phonon::${PHONON_LIB_SONAME} ALIAS ${PHONON_LIB_SONAME})

target_link_libraries(${PHONON_LIB_SONAME}
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Widgets
    PRIVATE
        ${optional_link_libraries}
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(${PHONON_LIB_SONAME} PRIVATE Qt6::Core5Compat)
endif()

set_target_properties(${PHONON_LIB_SONAME}
    PROPERTIES
        VERSION ${PHONON_VERSION}
        SOVERSION ${PHONON_SOVERSION}
        DEFINE_SYMBOL MAKE_PHONON_LIB)

target_include_directories(${PHONON_LIB_SONAME}
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
    PUBLIC
        # snake header <phonon/foo.h>
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../;${CMAKE_CURRENT_BINARY_DIR}/../>"
        # camel header <phonon/Foo>
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../includes;${CMAKE_CURRENT_BINARY_DIR}/../includes>")

install(TARGETS ${PHONON_LIB_SONAME} EXPORT PhononLibs ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

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
