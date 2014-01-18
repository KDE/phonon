include_directories(${CMAKE_BINARY_DIR}/includes/phonon)

# Mostly clean and refactored Phonon 5 sources
set(phonon_LIB_SRCS
    factory.cpp
    frontend.cpp
    player.cpp
    source.cpp
    sourcecontrol.cpp

    # Abstracts
    abstractoutput.cpp

    # Librarians
    phononversion.cpp

    # Privates (QTC Compat)
    abstractoutput_p.h
    factory_p.h
    frontend_p.h
    player_p.h
    source_p.h
    sourcecontrol_p.h

    # Interfaces (QTC Compat)
    backendinterface.h
    playerinterface.h
    sourcecontrolinterface.h
)

# random plunder battlefield4 sources
set(phonon_LIB_SRCS
${phonon_LIB_SRCS}
   abstractmediastream.cpp
#   audiodataoutput.cpp
   audiooutput.cpp
#    effect.cpp
#    effectparameter.cpp
#    effectwidget.cpp
   iodevicestream.cpp
   objectdescription.cpp
   phononglobal.cpp
#    seekslider.cpp
   streaminterface.cpp
#    swiftslider.cpp
#    volumefadereffect.cpp
#    volumeslider.cpp
#    videoplayer.cpp
    videowidget.cpp
    videoitem.cpp

# QTC Compat
    audiooutput_p.h
    globalstatic_p.h
    frontend_p.h
    phononpimpl_p.h
    videowidget_p.h
    videowidgetinterface.h

    audiooutputinterface.h
    videosurfaceinterface.h
   )

add_definitions(-DPHONON_LIBRARY_PATH="${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR}/plugins")

add_library(${PHONON_LIB_SONAME} SHARED ${phonon_LIB_SRCS})
qt5_use_modules(${PHONON_LIB_SONAME} Core Widgets Quick)
target_link_libraries(${PHONON_LIB_SONAME} ${OPENGL_LIBRARY})

set_target_properties(${PHONON_LIB_SONAME} PROPERTIES
                                           VERSION ${PHONON_LIB_VERSION}
                                           SOVERSION ${PHONON_LIB_SOVERSION}
                                           DEFINE_SYMBOL MAKE_PHONON_LIB
                                           LINK_INTERFACE_LIBRARIES "")
# LINK_INTERFACE_LIBRARIES: By default don't add any linked libraries to the "exported"
# link interfaces, so that executables linking against this library
# will not automatically add implicit dependencies to their link list.
# This reduces inter-package dependencies and makes it easier to remove
# dependencies of shared libraries without breaking binary compatibility.

install(TARGETS ${PHONON_LIB_SONAME} EXPORT PhononLibs ${INSTALL_TARGETS_DEFAULT_ARGS} )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononversion.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononversion.h)

install(FILES
#    abstractmediastream.h
    abstractoutput.h
    audiodataoutput.h
    audiodataoutputinterface.h
    audiooutput.h
    audiooutputinterface.h
    backendinterface.h
    effect.h
    effectinterface.h
    effectparameter.h
    effectwidget.h
    frontend.h
    globaldescriptioncontainer.h
    player.h
    playerinterface.h
    source.h
    sourcecontrol.h
    sourcecontrolinterface.h
    objectdescription.h
    phononexport.h
    ${CMAKE_CURRENT_BINARY_DIR}/phononversion.h
    seekslider.h
    streaminterface.h
    videoplayer.h
    videowidget.h
    videowidgetinterface.h
    volumefadereffect.h
    volumefaderinterface.h
    volumeslider.h
    ${phonon_VGO_HDRS}
    phononglobal.h

    videosurfaceinterface.h
    videoitem.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)
