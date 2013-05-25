project(Phonon)

cmake_minimum_required(VERSION 2.6.9 FATAL_ERROR)

option(PHONON_ASSERT_STATES "Enable code to assert backend state transitions" ON)
option(PHONON_BUILD_EXPERIMENTAL "Build the experimental library" ON)
option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DECLARATIVE_PLUGIN "Build the Qt Declarative (QML) plugin" ON)
option(PHONON_BUILD_DESCRIPTOR "Builds and installs a library descriptor to be used for ABI checks" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_DOC "Build the API documentation" OFF)

option(PHONON_NO_CAPTURE "Disable the capture capabilities")
option(PHONON_NO_DBUS "Deactivate DBus support (used to expose AudioOutputs and for deprecated runtime backend switching)" OFF)

option(PHONON_BUILD_PHONON4QT5 "Build Phonon 4 transitional library for Qt5 (API compatible with regular Phonon4)" OFF)

# The following variables directly influence the library's soname version.
# It is highly advised to think twice before changing those.
# If you are unsure about the this: http://plan99.net/~mike/writing-shared-libraries.html
set(PHONON_LIB_MAJOR_VERSION "4") # Only change on binary incompatible changes
set(PHONON_LIB_MINOR_VERSION "6") # Only change on binary compatible changes with new interfaces
set(PHONON_LIB_PATCH_VERSION "60") # Bump whenever you feel like it :P
set(PHONON_LIB_VERSION "${PHONON_LIB_MAJOR_VERSION}.${PHONON_LIB_MINOR_VERSION}.${PHONON_LIB_PATCH_VERSION}")
set(PHONON_LIB_SOVERSION ${PHONON_LIB_MAJOR_VERSION})
set(PHONON_LIB_SONAME phonon)
set(PHONON_LIB_SONAME_CAMEL Phonon)

if(PHONON_BUILD_PHONON4QT5)
    set(PHONON_INTERNAL_BUILD_QT5 true)
    set(PHONON_LIB_SONAME phonon4qt5)
    set(PHONON_LIB_SONAME_CAMEL Phonon4Qt5)
endif (PHONON_INTERNAL_BUILD_QT5)

if (PHONON_ASSERT_STATES)
    message(STATUS "Enabling backend state machine validation.")
    add_definitions(-DPHONON_ASSERT_STATES)
endif (PHONON_ASSERT_STATES)

set(PHONON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

################################# Requirements #################################
# Core Requirements and CFlag Magic
include(cmake/FindPhononInternal.cmake)

# Internal is shared with backends, so continue specific finders here...
find_package(OpenGL)
macro_log_feature(OPENGL_FOUND "OpenGL" "" "" FALSE)

if (NOT PHONON_INTERNAL_BUILD_QT5) # Qt 4
    macro_log_feature(QT_QTDBUS_FOUND "Qt4 DBus" "" "" FALSE)
    macro_log_feature(QT_QTDESIGNER_FOUND "Qt4 Designer" "" "" FALSE)
    macro_log_feature(QT_QTOPENGL_FOUND "Qt4 OpenGL" "" "" FALSE)
    macro_log_feature(QT_QTDECLARATIVE_FOUND "Qt4 Declarative" "" "" FALSE)
else (NOT PHONON_INTERNAL_BUILD_QT5) # Qt 5
    # DBus Audio Output Introspection
    find_package(Qt5DBus)
    macro_log_feature(Qt5DBus_FOUND "Qt5 DBus (qtbase)" "" "" FALSE)
    set(QT_QTDBUS_FOUND ${Qt5DBus_FOUND})

    # Designer Plugin
    find_package(Qt5Designer)
    macro_log_feature(Qt5Designer_FOUND "Qt5 Designer (qttools)" "" "" FALSE)
    set(QT_QTDESIGNER_FOUND ${Qt5Designer_FOUND})

    # VideoGraphicsObject GL Painting
    find_package(Qt5OpenGL)
    macro_log_feature(Qt5OpenGL_FOUND "Qt5 OpenGL (qtbase)" "" "" FALSE)
    set(QT_QTOPENGL_FOUND ${Qt5OpenGL_FOUND})

    # Declarative/QML1
    find_package(Qt5Declarative)
    macro_log_feature(Qt5Declarative_FOUND "Qt5 Declarative (qtquick1)" "" "" FALSE)
    set(QT_QTDECLARATIVE_FOUND ${Qt5Declarative_FOUND})
endif (NOT PHONON_INTERNAL_BUILD_QT5)
################################################################################

add_definitions(${QT_DEFINITIONS})
remove_definitions(-DQT3_SUPPORT_WARNINGS -DQT3_SUPPORT)

include_directories(${QT_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/includes
                    ${CMAKE_CURRENT_SOURCE_DIR}/phonon
                    ${CMAKE_CURRENT_BINARY_DIR}/phonon)

# Convenience variable to hold target link libraries we always need.
set(PHONON_LIBS ${PHONON_LIB_SONAME})

# helper macro to make the install paths absolute
macro(MAKE_ABS_INSTALL_PATH _absVar _path)
    if (IS_ABSOLUTE "${_path}")
        set(${_absVar} "${_path}")
    else()
        set(${_absVar} "${CMAKE_INSTALL_PREFIX}/${_path}")
    endif()
endmacro(MAKE_ABS_INSTALL_PATH)


set(BUILDSYSTEM_INSTALL_DIR ${SHARE_INSTALL_PREFIX}/${PHONON_LIB_SONAME}/buildsystem/)

include(GNUInstallDirs)

# Provide a cmake option, which, if set, force the Qt stuff to be installed into the
# system Qt directories, which may be outside CMAKE_INSTALL_PREFIX.
# By default always install inside CMAKE_INSTALL_PREFIX.
option(PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT "Install Qt designer plugins, QML plugins and mkspecs into the system Qt install directory or not"
       FALSE)

if(PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT)
    # in this case, the values have to be FORCED into the cache
    set(PHONON_QT_IMPORTS_INSTALL_DIR ${QT_IMPORTS_DIR} CACHE PATH "The directory where Phonon Qt imports (QML) will be installed to." FORCE)
    set(PHONON_QT_MKSPECS_INSTALL_DIR  ${QT_MKSPECS_DIR}/modules  CACHE PATH "The directory where Phonon mkspecs will be installed to." FORCE)
    set(PHONON_QT_PLUGIN_INSTALL_DIR   ${QT_PLUGINS_DIR}/designer CACHE PATH "The directory where Phonon Qt plugins will be installed to." FORCE)
else(PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT)
    # in this case, the values can be adjusted by the user e.g. via cmake-gui, so no FORCE
    set(PHONON_QT_IMPORTS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports CACHE PATH "The directory where Phonon Qt imports (QML) will be installed to.")
    set(PHONON_QT_MKSPECS_INSTALL_DIR  share/qt5/mkspecs/modules  CACHE PATH "The directory where Phonon mkspecs will be installed to.")
    set(PHONON_QT_PLUGIN_INSTALL_DIR   ${CMAKE_INSTALL_LIBDIR}/qt5/plugins/designer   CACHE PATH "The directory where Phonon Qt plugins will be installed to." )
endif(PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT)


# Check whether the QML plugins will be installed into the system Qt dir, and warn if not:
make_abs_install_path(absImportsDir "${PHONON_QT_IMPORTS_INSTALL_DIR}")

if(NOT "${absImportsDir}" STREQUAL "${QT_IMPORTS_DIR}")
    message(STATUS "PHONON_QT_IMPORTS_DIR is set to ${absImportsDir}.
      The QML plugins for Phonon will not be installed into the Qt system installation directory,
      which is ${QT_IMPORTS_DIR} .
      This means the QML plugins file will not be found by default.
      You can:
       * switch the cmake option PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT to TRUE
       * set PHONON_QT_IMPORTS_INSTALL_DIR manually to the system Qt location
       * at runtime, set the QML_IMPORT_PATH environment variable")
endif(NOT "${absImportsDir}" STREQUAL "${QT_IMPORTS_DIR}")


# Check whether the mkspecs file will be installed into the system Qt dir, and warn if not:
make_abs_install_path(absMkspecsDir "${PHONON_QT_MKSPECS_INSTALL_DIR}")

if(NOT "${absMkspecsDir}" STREQUAL "${QT_MKSPECS_DIR}/modules")
    message(STATUS "PHONON_QT_MKSPECS_INSTALL_DIR is set to ${absMkspecsDir}.
      The Qt mkspecs file for Phonon will not be installed into the Qt system installation directory,
      which is ${QT_MKSPECS_DIR}/modules .
      This means the qt_phonon.pri file will not be found by default.
      You can:
       * switch the cmake option PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT to TRUE
       * set PHONON_QT_MKSPECS_INSTALL_DIR manually to the system Qt location
       * at runtime, with Qt >= 4.8, set the QMAKEPATH environment variable")
endif(NOT "${absMkspecsDir}" STREQUAL "${QT_MKSPECS_DIR}/modules")


# Check whether the designer plugins will be installed into the system Qt dir, and warn if not:
make_abs_install_path(absPluginDir "${PHONON_QT_PLUGIN_INSTALL_DIR}")

if(NOT "${absPluginDir}" STREQUAL "${QT_PLUGINS_DIR}/designer")
    message(STATUS "PHONON_QT_PLUGIN_INSTALL_DIR is set to ${absPluginDir}.
      The Qt designer plugins for Phonon will not be installed into the Qt system installation directory,
      which is ${QT_PLUGINS_DIR}/designer .
      This means the designer plugins file will not be found by default.
      You can:
       * switch the cmake option PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT to TRUE
       * set PHONON_QT_PLUGINS_INSTALL_DIR manually to the system Qt location
       * at runtime, set the QT_PLUGIN_PATH environment variable")
endif(NOT "${absPluginDir}" STREQUAL "${QT_PLUGINS_DIR}/designer")



add_subdirectory(cmake)
if(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)

if(QT_QTDECLARATIVE_FOUND AND PHONON_BUILD_DECLARATIVE_PLUGIN)
    add_subdirectory(declarative)
endif(QT_QTDECLARATIVE_FOUND AND PHONON_BUILD_DECLARATIVE_PLUGIN)

if(PHONON_BUILD_DOC)
    add_subdirectory(doc)
endif(PHONON_BUILD_DOC)

add_subdirectory(phonon)
add_subdirectory(includes)

if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    #Allows find_package(Phonon) to not die in the demos
    set(PHONON_BUILDSYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    set(PHONON_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/includes/)
    set(PHONON_LIBRARY ${PHONON_LIB_SONAME})
    #Lets the demos find the headers
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(demos)
endif(PHONON_BUILD_DEMOS)

if(NOT WIN32) # pkgconfig file

    make_abs_install_path(ABS_LIB_INSTALL_DIR "${LIB_INSTALL_DIR}")
    make_abs_install_path(ABS_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}")
    make_abs_install_path(ABS_BUILDSYSTEM_INSTALL_DIR "${BUILDSYSTEM_INSTALL_DIR}")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phonon.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/${PHONON_LIB_SONAME} )

# figure out the relative path from the installed Config.cmake file to the install prefix (which may be at
# runtime different from the chosen CMAKE_INSTALL_PREFIX if under Windows the package was installed anywhere)
# This relative path will be configured into the PhononConfig.cmake
file(RELATIVE_PATH relInstallDir ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX} )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PhononConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PhononConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake @ONLY)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR} )

install(EXPORT PhononLibs DESTINATION ${CMAKECONFIG_INSTALL_DIR} NAMESPACE Phonon:: FILE PhononTargets.cmake )

if (NOT PHONON_INTERNAL_BUILD_QT5)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qt_phonon.pri ${CMAKE_CURRENT_BINARY_DIR}/qt_${PHONON_LIB_SONAME}.pri @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt_${PHONON_LIB_SONAME}.pri DESTINATION  ${PHONON_QT_MKSPECS_INSTALL_DIR})
else (NOT PHONON_INTERNAL_BUILD_QT5)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qt5_phonon.pri ${CMAKE_CURRENT_BINARY_DIR}/qt_${PHONON_LIB_SONAME}.pri @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt_${PHONON_LIB_SONAME}.pri DESTINATION  ${PHONON_QT_MKSPECS_INSTALL_DIR})
endif (NOT PHONON_INTERNAL_BUILD_QT5)

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
