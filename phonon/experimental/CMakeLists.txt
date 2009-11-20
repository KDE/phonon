add_subdirectory(tests)

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
    packet.cpp
    packetpool.cpp
    videodataoutput.cpp
    videodataoutput2.cpp
    videowidget.cpp
    visualization.cpp
    ../abstractaudiooutput_p.cpp
    ../abstractvideooutput_p.cpp
   )
automoc4_add_library(phononexperimental SHARED ${phononexperimental_LIB_SRCS})
target_link_libraries(phononexperimental ${PHONON_LIBS})
set_target_properties(phononexperimental PROPERTIES
                                         VERSION ${PHONON_LIB_VERSION}
                                         SOVERSION ${PHONON_LIB_SOVERSION}
                                         DEFINE_SYMBOL MAKE_PHONONEXPERIMENTAL_LIB
                                         )
install(TARGETS phononexperimental ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install headers ###############
install(FILES
    abstractaudiodataoutput.h
    abstractvideodataoutput.h
    audiodataoutput.h
    audiodataoutputinterface.h
    audioformat.h
    avcapture.h
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
