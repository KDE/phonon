add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(experimental)

set(phonon_LIB_SRCS
   objectdescription.cpp
   objectdescriptionmodel.cpp
   phononnamespace.cpp
   mediasource.cpp
   abstractmediastream.cpp
   #abstractmediastream2.cpp
   streaminterface.cpp
   mediaobject.cpp
   medianode.cpp
   path.cpp
   effectparameter.cpp
   effect.cpp
   volumefadereffect.cpp
   abstractaudiooutput.cpp
   abstractaudiooutput_p.cpp
   audiooutput.cpp
   abstractvideooutput.cpp
   abstractvideooutput_p.cpp
   backendcapabilities.cpp
   globalconfig.cpp
   factory.cpp
   platform.cpp
   mediacontroller.cpp
   videowidget.cpp
   videoplayer.cpp
   seekslider.cpp
   volumeslider.cpp
   effectwidget.cpp
   iodevicestream.cpp
   lockfreequeue.cpp
   streameventqueue.cpp
   )

if (QT_QTDBUS_FOUND)
   list(APPEND phonon_LIB_SRCS
      audiooutputadaptor.cpp
   )
endif (QT_QTDBUS_FOUND)


automoc4_add_library(phonon SHARED ${phonon_LIB_SRCS})
target_link_libraries(phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if (QT_QTDBUS_FOUND)
  target_link_libraries(phonon ${QT_QTDBUS_LIBRARY})
endif (QT_QTDBUS_FOUND)
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  # We need to explicitly link libm to phonon in Solaris
  target_link_libraries(phonon m)
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

set_target_properties(phonon PROPERTIES
                             VERSION ${PHONON_LIB_VERSION}
                             SOVERSION ${PHONON_LIB_SOVERSION}
                             DEFINE_SYMBOL MAKE_PHONON_LIB
                             )
install(TARGETS phonon ${INSTALL_TARGETS_DEFAULT_ARGS})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phononnamespace.h.in ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h)

install(FILES
   phonon_export.h
   objectdescription.h
   objectdescriptionmodel.h
   ${CMAKE_CURRENT_BINARY_DIR}/phononnamespace.h
   mediasource.h
   abstractmediastream.h
   #abstractmediastream2.h
   streaminterface.h
   mediaobject.h
   audiooutput.h
   medianode.h
   path.h
   effectparameter.h
   effect.h
   effectinterface.h
   volumefadereffect.h
   volumefaderinterface.h
   abstractaudiooutput.h
   abstractvideooutput.h
   backendcapabilities.h
   phonondefs.h
   backendinterface.h
   mediaobjectinterface.h
   audiooutputinterface.h
   addoninterface.h
   mediacontroller.h
   videowidget.h
   videowidgetinterface.h
   videoplayer.h
   seekslider.h
   volumeslider.h
   effectwidget.h
   platformplugin.h
   DESTINATION ${INCLUDE_INSTALL_DIR}/phonon COMPONENT Devel)

install(FILES org.kde.Phonon.AudioOutput.xml  DESTINATION  ${DBUS_INTERFACES_INSTALL_DIR})
