project(Phonon)

cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
add_definitions(-DPHONON_BUILD_WITH_CMAKE)

option(PHONON_ASSERT_STATES "Enable code to assert backend state transitions" ON)
option(PHONON_BUILD_EXPERIMENTAL "Build the experimental library" ON)
option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DECLARATIVE_PLUGIN "Build the Qt Declarative (QML) plugin" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_DOC "Build the API documentation" OFF)

option(PHONON_INSTALL_QT_COMPAT_HEADERS "Install QtPhonon compatibility headers" OFF)

option(PHONON_NO_CAPTURE "Disable the capture capabilities")
option(PHONON_NO_DBUS "Deactivate DBus support (used to expose AudioOutputs and for deprecated runtime backend switching)" OFF)

# Compat: we used to have our own option, now we defer to KDEInstallDirs
if(PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT)
    message(DEPRECATION "PHONON_INSTALL_QT_EXTENSIONS_INTO_SYSTEM_QT is deprecated! use KDE_INSTALL_USE_QT_SYS_PATHS")
    set(KDE_INSTALL_USE_QT_SYS_PATHS ON)
endif()

find_package(ECM 1.7.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMGeneratePriFile)

set(PHONON_LIB_SONAME phonon4qt5)
set(PHONON_LIB_SONAME_CAMEL Phonon4Qt5)
include(ECMSetupVersion)
ecm_setup_version("4.10.3"
    VARIABLE_PREFIX PHONON
    SOVERSION 4
    VERSION_HEADER phonon/phonon_version.h
    PACKAGE_VERSION_FILE ${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake
)

# Glorified write_basic_package_version_file call for experimental. Experimental
# inherits the versions from Phonon proper, so we technically don't need this.
ecm_setup_version(${PHONON_VERSION}
    VARIABLE_PREFIX PHONON_EXPERIMENTAL
    SOVERSION ${PHONON_SOVERSION}
    PACKAGE_VERSION_FILE ${PHONON_LIB_SONAME_CAMEL}ExperimentalConfigVersion.cmake
)

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

set(DBUS_DESC "Needed to enable control of AudioOutputs via DBus")
set(DESIGNER_DESC "Needed to build the Qt Designer plugin.")
set(GL_DESC "Only recommended when bulding the super experimental QML support.")
set(DECL_DESC "Only recommended when bulding the super experimental QML support.")

# DBus Audio Output Introspection
find_package(Qt5DBus)
macro_log_feature(Qt5DBus_FOUND "Qt5 DBus (qtbase)" ${DBUS_DESC} "http://www.qt.gitorious.net/qt/qtbase" FALSE)
set(QT_QTDBUS_FOUND ${Qt5DBus_FOUND})

# Designer Plugin
find_package(Qt5Designer)
macro_log_feature(Qt5Designer_FOUND "Qt5 Designer (qttools)" ${DESIGNER_DESC} "http://www.qt.gitorious.net/qt/qttools" FALSE)
set(QT_QTDESIGNER_FOUND ${Qt5Designer_FOUND})

# VideoGraphicsObject GL Painting
find_package(Qt5OpenGL)
macro_log_feature(Qt5OpenGL_FOUND "Qt5 OpenGL (qtbase)" ${GL_DESC} "http://www.qt.gitorious.net/qt/qtbase" FALSE)
set(QT_QTOPENGL_FOUND ${Qt5OpenGL_FOUND})

# Declarative/QML1
find_package(Qt5Declarative)
macro_log_feature(Qt5Declarative_FOUND "Qt5 Declarative (qtquick1)" ${DECL_DESC} "http://www.qt.gitorious.net/qt/qtquick1" FALSE)
set(QT_QTDECLARATIVE_FOUND ${Qt5Declarative_FOUND})

set(PHONON_PKG_DEPENDENCIES "Qt5Core Qt5Gui Qt5DBus")

if(NOT PHONON_BUILD_DECLARATIVE_PLUGIN OR NOT QT_QTOPENGL_FOUND OR NOT OPENGL_FOUND OR NOT QT_QTDECLARATIVE_FOUND)
    set(PHONON_NO_GRAPHICSVIEW true)
else()
    set(PHONON_NO_GRAPHICSVIEW false)
endif()

include_directories(
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/includes
                    ${CMAKE_CURRENT_SOURCE_DIR}/phonon
                    ${CMAKE_CURRENT_BINARY_DIR}/phonon)

# Convenience variable to hold target link libraries we always need.
set(PHONON_LIBS ${PHONON_LIB_SONAME})

include(GNUInstallDirs)

set(BUILDSYSTEM_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PHONON_LIB_SONAME}/buildsystem/)

add_subdirectory(cmake)
if(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)

if(NOT PHONON_NO_GRAPHICSVIEW)
    message(STATUS "Not building declarative plugin.")
    add_subdirectory(declarative)
endif()

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
    # helper macro to make the install paths absolute
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

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/phonon.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${PHONON_LIB_SONAME}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PhononConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake
               @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR} )

install(EXPORT PhononLibs
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        NAMESPACE Phonon::
        FILE PhononTargets.cmake )

if(PHONON_BUILD_EXPERIMENTAL)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PhononExperimentalConfig.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ExperimentalConfig.cmake
                   @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ExperimentalConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ExperimentalConfigVersion.cmake
            DESTINATION ${CMAKECONFIG_INSTALL_DIR})

    install(EXPORT PhononExperimentalLibs
            DESTINATION ${CMAKECONFIG_INSTALL_DIR}
            NAMESPACE Phonon::
            FILE PhononExperimentalTargets.cmake )
endif()

ecm_generate_pri_file(
   BASE_NAME ${PHONON_LIB_SONAME}
   LIB_NAME ${PHONON_LIB_SONAME}
   DEPS "core widgets"
   FILENAME_VAR pri_filename
   INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/phonon
)
install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if(ECM_FOUND AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

macro_display_feature_log()
