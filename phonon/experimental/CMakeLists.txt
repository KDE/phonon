add_subdirectory(tests)

########### libphononexperimental ###############
set(phononexperimental_LIB_SRCS
    audiodataoutput.cpp
    videodataoutput.cpp
    visualization.cpp
    mediasource.cpp
    backendcapabilities.cpp
    factory.cpp
    globalconfig.cpp
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
    export.h
    audiodataoutput.h
    videoframe.h
    videodataoutput.h
    visualization.h
    mediasource.h
    backendcapabilities.h
    backendinterface.h
    objectdescription.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/phonon/experimental COMPONENT Devel)
