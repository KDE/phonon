set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
if(msvc)
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
endif(msvc)
add_subdirectory(fakebackend)

if(PHONON_BUILD_APP_TESTS)
	add_subdirectory(captureapptest)
endif(PHONON_BUILD_APP_TESTS)

MACRO(PHONON_UNIT_TESTS)
	FOREACH(_testname ${ARGN})
		phonon_add_unit_test(${_testname} ${_testname}.cpp)
		target_link_libraries(${_testname} ${PHONON_LIBS} ${QT_QTTEST_LIBRARY})
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS)

# same as above but using the fake backend
MACRO(PHONON_UNIT_TESTS_FAKE)
	FOREACH(_testname ${ARGN})
		phonon_add_unit_test(${_testname}_fake ${_testname}.cpp)
      target_link_libraries(${_testname}_fake ${PHONON_LIBS} ${QT_QTTEST_LIBRARY} phonon_fake)
		SET_TARGET_PROPERTIES(${_testname}_fake PROPERTIES COMPILE_FLAGS -DUSE_FAKE_BACKEND)
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS_FAKE)

MACRO(PHONON_EXECUTABLE_TESTS)
   FOREACH(_testname ${ARGN})
      phonon_add_executable(${_testname} ${_testname}.cpp)
      target_link_libraries(${_testname} ${PHONON_LIBS} ${QT_QTTEST_LIBRARY})
   ENDFOREACH(_testname)
ENDMACRO(PHONON_EXECUTABLE_TESTS)

PHONON_EXECUTABLE_TESTS(
	mediaobjecttest
	pathtest
	backendcapabilitiestest
	audiooutputtest
	audiooutputdevicetest
	#audiocapturedevicetest
	#videocapturedevicetest
	#bytestreamtest
	#bytestreamtest2
	seekslidertest
	volumeslidertest
	audiodataoutputtest
	)

PHONON_UNIT_TESTS(
	)

PHONON_UNIT_TESTS_FAKE(
	mediaobjecttest
	pathtest
	backendcapabilitiestest
	audiooutputtest
	audiooutputdevicetest
	#audiocapturedevicetest
	#videocapturedevicetest
	#bytestreamtest
	#bytestreamtest2
	seekslidertest
	volumeslidertest
	audiodataoutputtest
)
target_link_libraries(backendcapabilitiestest ${PHONON_KDECORE_LIBS})
target_link_libraries(audiodevicelisttest ${PHONON_KAUDIODEVICELIST_LIBS})
target_link_libraries(mediaplayer ${PHONON_KIO_LIBS})
target_link_libraries(backendcapabilitiestest_fake ${PHONON_KDECORE_LIBS})
#methodtest does not link against libphonon
phonon_add_executable(methodtest methodtest.cpp ../factory.cpp
                                                ../mediaobject.cpp
                                                ../phononnamespace.cpp
                                                ../platform.cpp
                                                ../objectdescription.cpp
                                                )
target_link_libraries(methodtest ${QT_QTTEST_LIBRARY} ${PHONON_LIBS})
SET_TARGET_PROPERTIES(methodtest PROPERTIES COMPILE_FLAGS "-DPHONON_METHODTEST -DPHONON_EXPORT=")
phonon_add_unit_test(methodtest_fake methodtest.cpp ../factory.cpp
                                                    ../phononnamespace.cpp
                                                    ../mediaobject.cpp
                                                    ../platform.cpp
                                                    ../objectdescription.cpp
                                                    )
target_link_libraries(methodtest_fake ${QT_QTTEST_LIBRARY} ${PHONON_LIBS} phonon_fake)
SET_TARGET_PROPERTIES(methodtest_fake PROPERTIES COMPILE_FLAGS "-DUSE_FAKE_BACKEND -DPHONON_METHODTEST -DPHONON_EXPORT=")

#audiodevicelisttest does not link agains libphonon, instead it links agains kde stuff
phonon_add_executable(audiodevicelisttest audiodevicelisttest.cpp)
target_link_libraries(audiodevicelisttest ${QT_QTTEST_LIBRARY} ${PHONON_KDECORE_LIBS} kaudiodevicelist)

SET(mediasourcetest_SRCS mediasourcetest.cpp)
qt4_add_resources(mediasourcetest_SRCS testresources.qrc)
phonon_add_unit_test(mediasourcetest ${mediasourcetest_SRCS})
target_link_libraries(mediasourcetest ${PHONON_LIBS} ${QT_QTTEST_LIBRARY})

SET(backendtester_SRCS backendtester.cpp)
qt4_wrap_ui(backendtester_SRCS backendtester.ui)
phonon_add_executable(backendtester ${backendtester_SRCS})
target_link_libraries(backendtester ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
