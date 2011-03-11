project(Phonon)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

# CMP0002: we have multiple targets with the same name for the unit tests
cmake_policy(SET CMP0002 OLD)

option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_TESTS "Build the tests" OFF)

option(PHONON_NO_CAPTURE "Disable the capture capabilities")
if (PHONON_NO_CAPTURE)
    # Definitions to disable the compiling of any capture related components
    add_definitions(-DPHONON_NO_CAPTURE)
endif (PHONON_NO_CAPTURE)

if (PHONON_BUILD_TESTS)
    message(STATUS "Building tests.")
    enable_testing()
endif (PHONON_BUILD_TESTS)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PHONON_INTERNAL TRUE)
find_package(PhononInternal)

set(PHONON_LIB_MAJOR_VERSION "4")
set(PHONON_LIB_MINOR_VERSION "4")
set(PHONON_LIB_PATCH_VERSION "57")
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.4.0")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})

add_definitions(${QT_DEFINITIONS})
remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)

include_directories(${QT_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/includes
                    ${CMAKE_CURRENT_SOURCE_DIR}/phonon
                    ${CMAKE_CURRENT_BINARY_DIR}/phonon)

# Convenience variable to hold target link libraries we always need.
set(PHONON_LIBS phonon ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
if(QT_QTDBUS_FOUND)
    list(APPEND PHONON_LIBS phonon ${QT_QTDBUS_LIBRARY})
endif(QT_QTDBUS_FOUND)

if(${QT_QTDESIGNER_FOUND} AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif(${QT_QTDESIGNER_FOUND} AND PHONON_BUILD_DESIGNER_PLUGIN)
add_subdirectory(phonon)
add_subdirectory(includes)
if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    #Allows find_package(Phonon) to not die in the demos
    set(PHONON_BUILDSYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    #Lets the demos find the headers
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
    add_subdirectory(demos)
endif(PHONON_BUILD_DEMOS)

set(BUILDSYSTEM_INSTALL_DIR ${SHARE_INSTALL_PREFIX}/phonon-buildsystem/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindAutomoc4.cmake
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

if (NOT PHONON_MKSPECS_DIR)
    set(PHONON_MKSPECS_DIR ${QT_MKSPECS_DIR})
endif (NOT PHONON_MKSPECS_DIR)

# Don't do this, it makes Qt build phonon the next time it's built
#install(FILES qt_phonon.pri DESTINATION  ${PHONON_MKSPECS_DIR}/modules)

macro_display_feature_log()
