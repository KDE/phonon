add_subdirectory(tests)

########### libphononexperimental ###############
set(phononexperimental_LIB_SRCS
    abstractvideodataoutput.cpp
    audiodataoutput.cpp
    avcapture.cpp
    backendcapabilities.cpp
    factory.cpp
    globalconfig.cpp
    mediasource.cpp
    videodataoutput.cpp
    videodataoutput2.cpp
    videowidget.cpp
    visualization.cpp
    ../abstractaudiooutput_p.cpp
    ../abstractvideooutput_p.cpp
   )
automoc4(phononexperimental phononexperimental_LIB_SRCS)
add_library(phononexperimental SHARED ${phononexperimental_LIB_SRCS})
target_link_libraries(phononexperimental ${PHONON_LIBS})
set_target_properties(phononexperimental PROPERTIES
                                         VERSION ${PHONON_LIB_VERSION}
                                         SOVERSION ${PHONON_LIB_SOVERSION}
                                         DEFINE_SYMBOL MAKE_PHONONEXPERIMENTAL_LIB
                                         )
install(TARGETS phononexperimental ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install headers ###############
install(FILES
    abstractvideodataoutput.h
    audiodataoutput.h
    avcapture.h
    backendcapabilities.h
    backendinterface.h
    export.h
    mediasource.h
    objectdescription.h
    snapshotinterface.h
    videodataoutput.h
    videodataoutput2.h
    videodataoutputinterface.h
    videoframe.h
    videoframe2.h
    videowidget.h
    visualization.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/phonon/experimental COMPONENT Devel)
