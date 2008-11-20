set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

MACRO(PHONON_UNIT_TESTS)
	FOREACH(_testname ${ARGN})
		phonon_add_unit_test(${_testname} ${_testname}.cpp)
		target_link_libraries(${_testname} ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY})
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS)

# same as above but using the fake backend
MACRO(PHONON_UNIT_TESTS_FAKE)
	FOREACH(_testname ${ARGN})
		phonon_add_unit_test(${_testname}_fake ${_testname}.cpp)
		target_link_libraries(${_testname}_fake ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY} phonon_fake)
		SET_TARGET_PROPERTIES(${_testname}_fake PROPERTIES COMPILE_FLAGS -DUSE_FAKE_BACKEND)
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS_FAKE)

MACRO(PHONON_EXECUTABLE_TESTS)
   FOREACH(_testname ${ARGN})
      set(_nogui)
      set(_add_executable_param)
      if (Q_WS_MAC)
         set(_add_executable_param MACOSX_BUNDLE)
      endif (Q_WS_MAC)
      if (WIN32)
         set(_add_executable_param WIN32)
      endif (WIN32)
      if (NOT PHONON_BUILD_TESTS)
         set(_add_executable_param ${_add_executable_param} EXCLUDE_FROM_ALL)
      endif (NOT PHONON_BUILD_TESTS)
      set(_SRCS ${_testname}.cpp)
      automoc4_add_executable(${_testname} ${_add_executable_param} ${_SRCS})
      target_link_libraries(${_testname} ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY})
   ENDFOREACH(_testname)
ENDMACRO(PHONON_EXECUTABLE_TESTS)

PHONON_EXECUTABLE_TESTS(
	audiodataoutputtest
	)

PHONON_UNIT_TESTS_FAKE(
	audiodataoutputtest
	)
