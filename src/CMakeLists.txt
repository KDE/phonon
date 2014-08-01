include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/pulse
    ${CMAKE_CURRENT_SOURCE_DIR}/streaming
    ${CMAKE_CURRENT_SOURCE_DIR}/video

    ${CMAKE_CURRENT_BINARY_DIR}
)

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
    debug.cpp
    phononversion.cpp

    # Specifics
    audio/audiooutput.cpp
    streaming/abstractmediastream.cpp
    streaming/iodevicestream.cpp
    streaming/streaminterface.cpp
    video/videowidget.cpp

    # Privates (QTC Compat)
    abstractoutput_p.h
    debug_p.h
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
#   audiodataoutput.cpp
#    effect.cpp
#    effectparameter.cpp
#    effectwidget.cpp
   phononglobal.cpp
#    seekslider.cpp
#    swiftslider.cpp
#    volumefadereffect.cpp
#    volumeslider.cpp
    video/videoitem.cpp

# QTC Compat
    audio/audiooutput_p.h
    globalstatic_p.h
    frontend_p.h
    phononpimpl_p.h
    video/videowidget_p.h
    video/videowidgetinterface.h

    audio/audiooutputinterface.h
    video/videosurfaceinterface.h
   )

message(WARNING "WTF")
add_definitions(-DPHONON_LIBRARY_PATH="${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR}")

add_library(${PHONON_LIB_SONAME} SHARED ${phonon_LIB_SRCS})

target_link_libraries(${PHONON_LIB_SONAME}
    LINK_PUBLIC
        Qt5::Core
    LINK_PRIVATE
        Qt5::Widgets
        Qt5::Quick
        ${OPENGL_LIBRARY}
)

set_target_properties(${PHONON_LIB_SONAME} PROPERTIES
    VERSION ${PHONON_VERSION_STRING}
    SOVERSION ${PHONON_SOVERSION}
    DEFINE_SYMBOL MAKE_PHONON_LIB
    EXPORT_NAME Core
)

message(WARNING "probably want a macro for generate_headers")
#ecm_generate_headers(Phonon_CamelCase_HEADERS
#    HEADER_NAMES
#        Player
#        Source
#)

#ecm_generate_headers(Phonon_CamelCase_HEADERS
#    HEADER_NAMES
#        AudioOutput
#    RELATIVE audio
#)

#ecm_generate_headers(Phonon_CamelCase_HEADERS
#    HEADER_NAMES
#        VideoWidget
#    RELATIVE video
#)

install(TARGETS ${PHONON_LIB_SONAME} EXPORT PhononTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
#    abstractmediastream.h
    abstractoutput.h
    audio/audiodataoutput.h
    audio/audiodataoutputinterface.h
    audio/audiooutput.h
    audio/audiooutputinterface.h
    backendinterface.h
    effects/effect.h
    effects/effectinterface.h
    effects/effectparameter.h
    effects/effectwidget.h
    frontend.h
    globaldescriptioncontainer.h
    player.h
    playerinterface.h
    source.h
    sourcecontrol.h
    sourcecontrolinterface.h
    phononexport.h
    phononversion.h
    ${CMAKE_CURRENT_BINARY_DIR}/phononversiondata.h
    widgets/seekslider.h
    streaming/streaminterface.h
    video/videoplayer.h
    video/videowidget.h
    video/videowidgetinterface.h
    effects/volumefadereffect.h
    effects/volumefaderinterface.h
    widgets/volumeslider.h
    phononglobal.h

    video/videosurfaceinterface.h
    video/videoitem.h

    DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${PHONON_LIB_SONAME}/phonon
    COMPONENT Devel
)
