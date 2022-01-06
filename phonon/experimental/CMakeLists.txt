########### libphononexperimental ###############
set(phononexperimental_LIB_SRCS
    abstractaudiodataoutput.cpp
    abstractvideodataoutput.cpp
    audiodataoutput.cpp
    audioformat.cpp
    avcapture.cpp
    backendcapabilities.cpp
    factory.cpp
    globalconfig.cpp
    mediasource.cpp
    objectdescription.cpp
    packet.cpp
    packetpool.cpp
    videodataoutput.cpp
    videodataoutput2.cpp
    videowidget.cpp
    visualization.cpp
    ../abstractaudiooutput_p.cpp
    ../abstractvideooutput_p.cpp
   )

add_library(${PHONON_LIB_SONAME}experimental SHARED ${phononexperimental_LIB_SRCS})
target_link_libraries(${PHONON_LIB_SONAME}experimental
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Widgets
        ${PHONON_LIB_SONAME}
)
set_target_properties(${PHONON_LIB_SONAME}experimental
    PROPERTIES
        VERSION ${PHONON_VERSION}
        SOVERSION ${PHONON_SOVERSION}
        DEFINE_SYMBOL MAKE_PHONONEXPERIMENTAL_LIB
)
install(TARGETS ${PHONON_LIB_SONAME}experimental EXPORT PhononExperimentalLibs ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
target_include_directories(${PHONON_LIB_SONAME}experimental
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>")

########### install headers ###############
install(FILES
    abstractaudiodataoutput.h
    abstractvideodataoutput.h
    audiodataoutput.h
    audiodataoutputinterface.h
    audioformat.h
    avcapture.h
    avcaptureinterface.h
    backendcapabilities.h
    backendinterface.h
    export.h
    mediasource.h
    objectdescription.h
    snapshotinterface.h
    packet.h
    packetpool.h
    phononnamespace.h
    videodataoutput.h
    videodataoutput2.h
    videodataoutputinterface.h
    videoframe.h
    videoframe2.h
    videowidget.h
    visualization.h
    globalconfig.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/phonon/experimental COMPONENT Devel)
