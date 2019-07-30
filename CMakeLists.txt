cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Phonon VERSION "4.10.60")

include(FeatureSummary)

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

find_package(ECM 5.60 NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Extra CMake Modules"
    URL "https://api.kde.org/frameworks/extra-cmake-modules/html/index.html")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(CMakePackageConfigHelpers)
include(ECMGeneratePriFile)
include(ECMGeneratePkgConfigFile)
include(KDEInstallDirs)
include(ECMPoQmTools)
include(KDECMakeSettings)
include(KDECompilerSettings)

set(PHONON_LIB_SONAME phonon4qt5)
set(PHONON_LIB_SONAME_CAMEL Phonon4Qt5)

include(ECMSetupVersion)
ecm_setup_version(PROJECT
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

set(INCLUDE_INSTALL_DIR "${KDE_INSTALL_INCLUDEDIR}/${PHONON_LIB_SONAME}")
set(PLUGIN_INSTALL_DIR ${KDE_INSTALL_QTPLUGINDIR})
if(WIN32) # Imported from Phonon VLC
    set(PLUGIN_INSTALL_DIR ${KDE_INSTALL_BINDIR})
endif()
set(BACKEND_DIR_SUFFIX "${PHONON_LIB_SONAME}_backend")
set(BACKEND_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/${BACKEND_DIR_SUFFIX}")

set(PHONON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

################################# Requirements #################################
# Core Requirements
find_package(Qt5Core)
set_package_properties(Qt5Core PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Qt5 core module"
    URL "https://www.qt.io/download")

find_package(Qt5Gui)
set_package_properties(Qt5Gui PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Qt5 GUI module"
    URL "https://www.qt.io/download")

find_package(Qt5Widgets)
set_package_properties(Qt5Widgets PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Qt5 widgets module"
    URL "https://www.qt.io/download")

# Internal is shared with backends, so continue specific finders here...
find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES
    TYPE RECOMMENDED
    URL "https://mesa3d.org")

# DBus Audio Output Introspection
find_package(Qt5DBus)
set_package_properties(Qt5DBus PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Needed to enable control of AudioOutputs via DBus"
    URL "https://www.qt.io/download")
message(AUTHOR_WARNING "TODO: port everything from deprecated names")
set(QT_QTDBUS_FOUND ${Qt5DBus_FOUND})

# Designer Plugin
find_package(Qt5Designer)
set_package_properties(Qt5Designer PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Needed to build the Qt Designer plugin"
    URL "https://www.qt.io/download")
set(QT_QTDESIGNER_FOUND ${Qt5Designer_FOUND})

# VideoGraphicsObject GL Painting
find_package(Qt5OpenGL)
set_package_properties(Qt5OpenGL PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Only recommended when bulding the super experimental QML support"
    URL "https://www.qt.io/download")
set(QT_QTOPENGL_FOUND ${Qt5OpenGL_FOUND})

# Declarative/QML1
find_package(Qt5Declarative)
set_package_properties(Qt5Declarative PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Only recommended when bulding the super experimental QML support"
    URL "https://www.qt.io/download")
set(QT_QTDECLARATIVE_FOUND ${Qt5Declarative_FOUND})

set(PHONON_PKG_DEPENDENCIES "Qt5Core Qt5Gui Qt5DBus")

if(NOT PHONON_BUILD_DECLARATIVE_PLUGIN OR NOT QT_QTOPENGL_FOUND OR NOT OPENGL_FOUND OR NOT QT_QTDECLARATIVE_FOUND)
    set(PHONON_NO_GRAPHICSVIEW true)
else()
    set(PHONON_NO_GRAPHICSVIEW false)
endif()

# Convenience variable to hold target link libraries we always need.
set(PHONON_LIBS ${PHONON_LIB_SONAME})

set(BUILDSYSTEM_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PHONON_LIB_SONAME}/buildsystem/)

add_subdirectory(cmake)
if(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif()

if(NOT PHONON_NO_GRAPHICSVIEW)
    message(STATUS "Not building declarative plugin.")
    add_subdirectory(declarative)
endif()

if(PHONON_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_subdirectory(phonon)
add_subdirectory(includes)

if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    #Allows find_package(Phonon) to not die in the demos
    set(PHONON_BUILDSYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    set(PHONON_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/includes/)
    set(PHONON_LIBRARY ${PHONON_LIB_SONAME})
    # Demos may also be built standalone, so we need to skip their find_package call!
    set(PHONON_FOUND TRUE)
    add_subdirectory(demos)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${PHONON_LIB_SONAME}")

configure_package_config_file(
    PhononConfig.cmake.in
    ${PHONON_LIB_SONAME_CAMEL}Config.cmake
    PATH_VARS INCLUDE_INSTALL_DIR KDE_INSTALL_LIBDIR
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        COMPONENT Devel)

install(EXPORT PhononLibs
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        NAMESPACE Phonon::
        FILE PhononTargets.cmake)

if(PHONON_BUILD_EXPERIMENTAL)
    configure_package_config_file(
        PhononExperimentalConfig.cmake.in
        ${PHONON_LIB_SONAME_CAMEL}ExperimentalConfig.cmake
        PATH_VARS INCLUDE_INSTALL_DIR KDE_INSTALL_LIBDIR
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ExperimentalConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ExperimentalConfigVersion.cmake
            DESTINATION ${CMAKECONFIG_INSTALL_DIR}
            COMPONENT Devel
    )

    install(EXPORT PhononExperimentalLibs
            DESTINATION ${CMAKECONFIG_INSTALL_DIR}
            NAMESPACE Phonon::
            FILE PhononExperimentalTargets.cmake)
endif()

ecm_generate_pkgconfig_file(
    BASE_NAME ${PHONON_LIB_SONAME}
    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
    DESCRIPTION "Phonon library needed to build applications"
    INSTALL
)

ecm_generate_pri_file(
   BASE_NAME ${PHONON_LIB_SONAME}
   LIB_NAME ${PHONON_LIB_SONAME}
   DEPS "core widgets"
   FILENAME_VAR pri_filename
   INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/phonon
)
install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
