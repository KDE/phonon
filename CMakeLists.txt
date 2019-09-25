cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Phonon VERSION "4.11.1")

include(FeatureSummary)

option(PHONON_BUILD_EXPERIMENTAL "Build the experimental library" ON)
option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_DOC "Build the API documentation" OFF)
option(PHONON_BUILD_SETTINGS "Build the settings GUI" ON)

option(PHONON_NO_CAPTURE "Disable the capture capabilities")

# Not set by ECM (yet)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

find_package(ECM 5.60 NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Extra CMake Modules"
    URL "https://api.kde.org/frameworks/extra-cmake-modules/html/index.html")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(CMakePackageConfigHelpers)
include(ECMGeneratePriFile)
include(ECMGeneratePkgConfigFile)
include(ECMPoQmTools)

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

# Designer Plugin
find_package(Qt5Designer)
set_package_properties(Qt5Designer PROPERTIES
    TYPE RECOMMENDED
    DESCRIPTION "Needed to build the Qt Designer plugin"
    URL "https://www.qt.io/download")

set(BUILDSYSTEM_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PHONON_LIB_SONAME}/buildsystem/)

add_subdirectory(cmake)
if(Qt5Designer_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif()

if(PHONON_BUILD_DOC)
    add_subdirectory(doc)
endif()

add_subdirectory(phonon)
add_subdirectory(includes)

if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    # Demos may also be built standalone, so we need to skip their find_package call!
    set(CMAKE_DISABLE_FIND_PACKAGE_Phonon4Qt5 TRUE)
    add_subdirectory(demos)
endif()

if(PHONON_BUILD_SETTINGS)
    add_subdirectory(settings)
endif()

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${PHONON_LIB_SONAME}")

configure_package_config_file(
    PhononConfig.cmake.in
    ${PHONON_LIB_SONAME_CAMEL}Config.cmake
    PATH_VARS
        INCLUDE_INSTALL_DIR
        KDE_INSTALL_LIBDIR
        BUILDSYSTEM_INSTALL_DIR
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
        PATH_VARS
            INCLUDE_INSTALL_DIR
            KDE_INSTALL_LIBDIR
            BUILDSYSTEM_INSTALL_DIR
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
   INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
)
install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
