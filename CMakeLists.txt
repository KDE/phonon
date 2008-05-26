project(Phonon)

cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

# enable unit tests
enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(MacroLogFeature)
include(MacroOptionalFindPackage)

set(QT_MIN_VERSION 4.4.0)
find_package(Qt4 REQUIRED)
if (NOT QT_QTDBUS_FOUND)
   message(FATAL_ERROR  "Phonon requires Qt4 with QtDBus module in order to built.")
endif(NOT QT_QTDBUS_FOUND)

find_package(Automoc4 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_COLOR_MAKEFILE ON)

set(PHONON_LIB_MAJOR_VERSION "4")
set(PHONON_LIB_MINOR_VERSION "2")
set(PHONON_LIB_PATCH_VERSION "0")
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.${PHONON_LIB_MINOR_VERSION}.0")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})

add_definitions(${QT_DEFINITIONS})
remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)
if(msvc)
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
endif(msvc)

# for including config.h and for includes like <kparts/foo.h>
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/includes ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/phonon)

option(PHONON_BUILD_TESTS "Build the tests")

macro(_SET_FANCY _var _value _comment)
   if (KDESupport_SOURCE_DIR)
      # when building inside kdesupport other subprojects write crap into our variables
      set(${_var} "${_value}")
   else (KDESupport_SOURCE_DIR)
      if (NOT DEFINED ${_var})
         set(${_var} "${_value}")
      else (NOT DEFINED ${_var})
         set(${_var} "${${_var}}" CACHE PATH "${_comment}")
      endif (NOT DEFINED ${_var})
   endif (KDESupport_SOURCE_DIR)
endmacro(_SET_FANCY)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")            # The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})

_set_fancy(BIN_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/bin"                "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
_set_fancy(LIB_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}"   "The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})")
_set_fancy(INCLUDE_INSTALL_DIR         "${CMAKE_INSTALL_PREFIX}/include"           "The subdirectory to the header prefix")
_set_fancy(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/kde4"                   "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
_set_fancy(SHARE_INSTALL_PREFIX        "${CMAKE_INSTALL_PREFIX}/share"             "Base directory for files which go to share/")
_set_fancy(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde4/services"     "The install dir for service (desktop, protocol, ...) files")
_set_fancy(DBUS_INTERFACES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" "The dbus interfaces install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
_set_fancy(DBUS_SERVICES_INSTALL_DIR   "${SHARE_INSTALL_PREFIX}/dbus-1/services"   "The dbus services install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )

# on the Mac support an extra install directory for application bundles
if(APPLE)
   if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.6")
      set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                                  BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
   endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" STREQUAL "2.6")
	set(CMAKE_INSTALL_NAME_DIR ${LIB_DESTINATION})
endif(APPLE)

if (CMAKE_SYSTEM_NAME MATCHES Linux)
   if (CMAKE_COMPILER_IS_GNUCXX)
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")

      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_MODULE_LINKER_FLAGS}")
      set ( CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_EXE_LINKER_FLAGS}")

      # we profile...
      if(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
      endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
   endif (CMAKE_COMPILER_IS_GNUCXX)
   if (CMAKE_C_COMPILER MATCHES "icc")
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
   endif (CMAKE_C_COMPILER MATCHES "icc")
endif (CMAKE_SYSTEM_NAME MATCHES Linux)

set(PHONON_LIBS phonon ${QT_QTDBUS_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set(_global_add_executable_param)
if (Q_WS_MAC)
   set(_global_add_executable_param MACOSX_BUNDLE)
endif (Q_WS_MAC)
if (WIN32)
   # no WIN32 here - all executables are command line executables
   set(_global_add_executable_param)
endif (WIN32)

macro(phonon_add_executable _target)
   set(_srcs ${ARGN})
   automoc4(${_target} _srcs)
   add_executable(${_target} ${_global_add_executable_param} ${_srcs})
endmacro(phonon_add_executable _target)

macro (PHONON_ADD_UNIT_TEST _test_NAME)
   set(_srcList ${ARGN})
   set(_nogui)
   list(GET ${_srcList} 0 first_PARAM)
   set(_add_executable_param ${_global_add_executable_param})
   if(${first_PARAM} STREQUAL "NOGUI")
      set(_nogui "NOGUI")
      set(_add_executable_param)
   endif(${first_PARAM} STREQUAL "NOGUI")

   if (NOT PHONON_BUILD_TESTS)
      set(_add_executable_param ${_add_executable_param} EXCLUDE_FROM_ALL)
   endif (NOT PHONON_BUILD_TESTS)

   automoc4(${_test_NAME} _srcList)
   add_executable(${_test_NAME} ${_add_executable_param} ${_srcList})

   if(NOT PHONON_TEST_OUTPUT)
      set(PHONON_TEST_OUTPUT plaintext)
   endif(NOT PHONON_TEST_OUTPUT)
   set(PHONON_TEST_OUTPUT ${PHONON_TEST_OUTPUT} CACHE STRING "The output to generate when running the QTest unit tests")

   set(using_qtest "")
   foreach(_filename ${_srcList})
      if(NOT using_qtest)
         if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
            file(READ ${_filename} file_CONTENT)
            string(REGEX MATCH "QTEST_(KDE)?MAIN" using_qtest "${file_CONTENT}")
         endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
      endif(NOT using_qtest)
   endforeach(_filename)

   set(_executable ${EXECUTABLE_OUTPUT_PATH}/${_test_NAME})
   if (Q_WS_MAC AND NOT _nogui)
      set(_executable ${EXECUTABLE_OUTPUT_PATH}/${_test_NAME}.app/Contents/MacOS/${_test_NAME})
   else (Q_WS_MAC AND NOT _nogui)
      # Use .shell wrapper where available, to use uninstalled libs.
      if (UNIX)
         set(_executable ${_executable}.shell)
      endif (UNIX)
   endif (Q_WS_MAC AND NOT _nogui)

   if (using_qtest AND PHONON_TEST_OUTPUT STREQUAL "xml")
      add_test( ${_test_NAME} ${_executable} -xml -o ${_test_NAME}.tml)
   else (using_qtest AND PHONON_TEST_OUTPUT STREQUAL "xml")
      add_test( ${_test_NAME} ${_executable} )
   endif (using_qtest AND PHONON_TEST_OUTPUT STREQUAL "xml")

   if (NOT MSVC_IDE)   #not needed for the ide
      # if the tests are EXCLUDE_FROM_ALL, add a target "buildtests" to build all tests
      if (NOT PHONON_BUILD_TESTS)
         get_directory_property(_buildtestsAdded BUILDTESTS_ADDED)
         if(NOT _buildtestsAdded)
            add_custom_target(buildtests)
            set_directory_properties(PROPERTIES BUILDTESTS_ADDED TRUE)
         endif(NOT _buildtestsAdded)
         add_dependencies(buildtests ${_test_NAME})
      endif (NOT PHONON_BUILD_TESTS)
   endif (NOT MSVC_IDE)
endmacro (PHONON_ADD_UNIT_TEST)

set(EXECUTABLE_OUTPUT_PATH ${Phonon_BINARY_DIR}/bin)
if (WIN32)
   set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
else (WIN32)
   set(LIBRARY_OUTPUT_PATH ${Phonon_BINARY_DIR}/lib)
endif (WIN32)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
   set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")

add_subdirectory(cmake)
add_subdirectory(phonon)
add_subdirectory(includes)
if (Q_WS_MAC)
  add_subdirectory(qt7)
endif (Q_WS_MAC)
if (Q_WS_WIN)
   add_subdirectory(ds9)
endif (Q_WS_WIN)
if (Q_WS_X11)
   add_subdirectory(gstreamer)
endif (Q_WS_X11)

macro_display_feature_log()
