project(Phonon)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

# CMP0002: we have multiple targets with the same name for the unit tests
cmake_policy(SET CMP0002 OLD)

# enable unit tests
option(PHONON_BUILD_TESTS "Build the tests")

# enable application tests
option(PHONON_BUILD_APP_TESTS "Build the application tests")

if (PHONON_BUILD_TESTS)
    message(STATUS "Building tests.")
    enable_testing()
endif (PHONON_BUILD_TESTS)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PhononInternal)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
set(CMAKE_COLOR_MAKEFILE ON)

set(PHONON_LIB_MAJOR_VERSION "4")
set(PHONON_LIB_MINOR_VERSION "4")
set(PHONON_LIB_PATCH_VERSION "4")
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.4.0")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})

add_definitions(${QT_DEFINITIONS})
remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)
if(MSVC)
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
endif(MSVC)

# for including config.h and for includes like <kparts/foo.h>
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/includes ${CMAKE_CURRENT_SOURCE_DIR}/phonon ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/phonon)

set(PHONON_LIBS phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if(QT_QTDBUS_FOUND)
    list(APPEND PHONON_LIBS phonon ${QT_QTDBUS_LIBRARY})
endif(QT_QTDBUS_FOUND)

set(PHONON_KDECORE_LIBS kdecore)

add_subdirectory(phonon)
add_subdirectory(includes)

set(BUILDSYSTEM_INSTALL_DIR ${SHARE_INSTALL_PREFIX}/phonon-buildsystem/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindAutomoc4.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPackageHandleStandardArgs.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPhonon.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPhononInternal.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PhononMacros.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindQt4.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MacroEnsureVersion.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MacroLogFeature.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MacroOptionalFindPackage.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MacroPushRequiredVars.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PhononMacros.cmake
        DESTINATION ${BUILDSYSTEM_INSTALL_DIR})

if(NOT WIN32) # pkgconfig file
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phonon.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/phonon.pc @ONLY)
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/phonon.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

install(FILES qt_phonon.pri DESTINATION  ${QT_MKSPECS_DIR}/modules)

macro_display_feature_log()
