project(Phonon)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

# CMP0002: we have multiple targets with the same name for the unit tests
cmake_policy(SET CMP0002 OLD)

option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DESCRIPTOR "Builds and installs a library descriptor to be used for ABI checks" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_TESTS "Build the tests" OFF)

option(PHONON_NO_CAPTURE "Disable the capture capabilities")
option(PHONON_NO_DBUS "Deactivate DBus support (used to expose AudioOutputs and for deprecated runtime backend switching)" OFF)

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

# The following variables directly influence the library's soname version.
# It is highly advised to think twice before changing those.
# If you are unsure about the this: http://plan99.net/~mike/writing-shared-libraries.html
set(PHONON_LIB_MAJOR_VERSION "4") # Only change on binary incompatible changes
set(PHONON_LIB_MINOR_VERSION "5") # Only change on binary compatible changes with new interfaces
set(PHONON_LIB_PATCH_VERSION "55") # Bump whenever you feel like it :P
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.${PHONON_LIB_MINOR_VERSION}.${PHONON_LIB_PATCH_VERSION}")
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

set(BUILDSYSTEM_INSTALL_DIR ${SHARE_INSTALL_PREFIX}/phonon-buildsystem/)

add_subdirectory(cmake)
if(${QT_QTDESIGNER_FOUND} AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif(${QT_QTDESIGNER_FOUND} AND PHONON_BUILD_DESIGNER_PLUGIN)
add_subdirectory(phonon)
add_subdirectory(includes)

if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    #Allows find_package(Phonon) to not die in the demos
    set(PHONON_BUILDSYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    set(PHONON_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(PHONON_LIBRARY phonon)
    #Lets the demos find the headers
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(demos)
endif(PHONON_BUILD_DEMOS)

if(NOT WIN32) # pkgconfig file

    # helper macro to make the install paths absolute for pkgconfig
    macro(MAKE_ABS_INSTALL_PATH _absVar _path)
        if (IS_ABSOLUTE "${_path}")
            set(${_absVar} "${_path}")
        else()
            set(${_absVar} "${CMAKE_INSTALL_PREFIX}/${_path}")
        endif()
    endmacro(MAKE_ABS_INSTALL_PATH)

    make_abs_install_path(ABS_LIB_INSTALL_DIR "${LIB_INSTALL_DIR}")
    make_abs_install_path(ABS_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}")
    make_abs_install_path(ABS_BUILDSYSTEM_INSTALL_DIR "${BUILDSYSTEM_INSTALL_DIR}")
  
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phonon.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/phonon.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/phonon.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PhononConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PhononConfig.cmake @ONLY)

set(PROJECT_VERSION_MAJOR ${PHONON_LIB_MAJOR_VERSION})
set(PROJECT_VERSION_MINOR ${PHONON_LIB_MINOR_VERSION})
set(PROJECT_VERSION_PATCH ${PHONON_LIB_PATCH_VERSION})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BasicFindPackageVersion.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/PhononConfigVersion.cmake" @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PhononConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/PhononConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/phonon )

install(EXPORT PhononLibs DESTINATION ${LIB_INSTALL_DIR}/cmake/phonon/ NAMESPACE Phonon:: FILE PhononTargets.cmake )


set(PHONON_MKSPECS_DIR ${QT_MKSPECS_DIR} CACHE PATH "The directory where Phonon mkspecs will be installed to.")

install(FILES qt_phonon.pri DESTINATION  ${PHONON_MKSPECS_DIR}/modules)

# This generates a nice library descriptor to use with [1]. It also spits out
# a script that makes installing various versions for an ABI check a lot easier.
# Basically the script ends up in your build dir and by running it you will
# get phonon installed to MAIN_SOURCE_DIR/../abi/VERSION/prefix.
# You can then invoke the ABI check with something like:
#   abi-compliance-checker.pl -l phonon -d1  4.4.4/usr/4.4.4.xml -d2 4.4.57/usr/4.4.57.xml
# [1] http://ispras.linux-foundation.org/index.php/ABI_compliance_checker
if(PHONON_BUILD_DESCRIPTOR)
    set(DESCRIPTOR_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_VERSION}.xml)
    set(ABI_SCRIPT_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR}/abi-check-install.sh)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib-descriptor.xml.cmake ${DESCRIPTOR_FILE_PATH} @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/abi-check-install.sh.cmake ${ABI_SCRIPT_FILE_PATH} @ONLY)
    install(FILES ${DESCRIPTOR_FILE_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(PHONON_BUILD_DESCRIPTOR)

macro_display_feature_log()
