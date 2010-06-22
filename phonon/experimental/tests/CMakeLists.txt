set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

MACRO(PHONON_UNIT_TESTS)
	FOREACH(_testname ${ARGN})
      set(srcs ${_testname}.cpp)
      if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
         qt4_add_resources(srcs ${_testname}.qrc)
      endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
      phonon_add_unit_test(${_testname} ${srcs})
		target_link_libraries(${_testname} ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY})
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS)

# same as above but using the fake backend
MACRO(PHONON_UNIT_TESTS_FAKE)
	FOREACH(_testname ${ARGN})
      set(srcs ${_testname}.cpp)
      if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
         qt4_add_resources(srcs ${_testname}.qrc)
      endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
      phonon_add_unit_test(${_testname}_fake ${srcs})
		target_link_libraries(${_testname}_fake ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY} phonon_fake)
		SET_TARGET_PROPERTIES(${_testname}_fake PROPERTIES COMPILE_FLAGS -DUSE_FAKE_BACKEND)
	ENDFOREACH(_testname)
ENDMACRO(PHONON_UNIT_TESTS_FAKE)

MACRO(PHONON_EXECUTABLE_TESTS)
   FOREACH(_testname ${ARGN})
      set(_add_executable_param)
      if (Q_WS_MAC)
         set(_add_executable_param MACOSX_BUNDLE)
      endif (Q_WS_MAC)
# this breaks compilation      
#      if (WIN32)
#         set(_add_executable_param WIN32)
#      endif (WIN32)
      if (NOT PHONON_BUILD_TESTS)
         set(_add_executable_param ${_add_executable_param} EXCLUDE_FROM_ALL)
      endif (NOT PHONON_BUILD_TESTS)
      set(srcs ${_testname}.cpp)
      if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
         qt4_add_resources(srcs ${_testname}.qrc)
      endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.qrc)
      automoc4_add_executable(${_testname} ${_add_executable_param} ${srcs})
      target_link_libraries(${_testname} ${PHONON_LIBS} phononexperimental ${QT_QTTEST_LIBRARY})
   ENDFOREACH(_testname)
ENDMACRO(PHONON_EXECUTABLE_TESTS)

PHONON_EXECUTABLE_TESTS(
   videodataoutputtest
   avcapturetest
	)

PHONON_UNIT_TESTS_FAKE(
	)
