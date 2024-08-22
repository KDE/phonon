cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Phonon VERSION "4.11.1")

include(FeatureSummary)

option(PHONON_BUILD_EXPERIMENTAL "Build the experimental library" ON)
option(PHONON_BUILD_DEMOS "Build the demos" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" ON)
option(PHONON_BUILD_DOC "Build the API documentation" OFF)
option(PHONON_BUILD_SETTINGS "Build the settings GUI" ON)
option(PHONON_BUILD_QT5 "Build for Qt5" ON)
option(PHONON_BUILD_QT6 "Build for Qt6" ON)
option(PHONON_NO_CAPTURE "Disable the capture capabilities")

# CI is stupid and doesn't allow us to set CMAKE options per build variant
if($ENV{CI_JOB_NAME_SLUG} MATCHES "qt5")
    set(PHONON_BUILD_QT5 ON)
    set(PHONON_BUILD_QT6 OFF)
elseif($ENV{CI_JOB_NAME_SLUG} MATCHES "qt6")
    set(PHONON_BUILD_QT5 OFF)
    set(PHONON_BUILD_QT6 ON)
endif()

set(VERSIONS)
if(PHONON_BUILD_QT5)
    list(APPEND VERSIONS "5")
endif()
if(PHONON_BUILD_QT6)
    list(APPEND VERSIONS "6")
    set(BUILD_WITH_QT6 ON) # necessary on older ECMs
endif()

# Not set by ECM (yet)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

set(QT_NO_CREATE_VERSIONLESS_TARGETS ON)
set(QT_NO_CREATE_VERSIONLESS_FUNCTIONS ON)

find_package(ECM 5.90 NO_MODULE)
set_package_properties(ECM PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Extra CMake Modules"
    URL "https://api.kde.org/frameworks/extra-cmake-modules/html/index.html")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
include(KDECompilerSettings NO_POLICY_SCOPE)

include(CMakePackageConfigHelpers)
include(ECMGeneratePkgConfigFile)
include(ECMPoQmTools)
include(ECMSetupVersion)

foreach(version ${VERSIONS})
    set(QT_MAJOR_VERSION ${version})

    include(KDEInstallDirs${version})
    include(KDECMakeSettings)
    include(ECMGeneratePriFile)

    set(PHONON_LIB_SONAME phonon4qt${QT_MAJOR_VERSION})
    set(PHONON_LIB_SONAME_CAMEL Phonon4Qt${QT_MAJOR_VERSION})

    ecm_setup_version(PROJECT
        VARIABLE_PREFIX PHONON
        SOVERSION 4
        VERSION_HEADER ${PHONON_LIB_SONAME}/phonon_version.h
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

    ################################# Requirements #################################

    set(REQUIRED_QT_VERSION 5.15.0)

    # Core Requirements
    find_package(Qt${QT_MAJOR_VERSION}Core ${REQUIRED_QT_VERSION})
    set_package_properties(Qt${QT_MAJOR_VERSION}Core PROPERTIES
        TYPE REQUIRED
        DESCRIPTION "Qt${QT_MAJOR_VERSION} core module"
        URL "https://www.qt.io/download")
    # https://bugreports.qt.io/browse/QTBUG-114706
    add_library(Qt::Core ALIAS Qt${QT_MAJOR_VERSION}::Core)

    find_package(Qt${QT_MAJOR_VERSION}Gui ${REQUIRED_QT_VERSION})
    set_package_properties(Qt${QT_MAJOR_VERSION}Gui PROPERTIES
        TYPE REQUIRED
        DESCRIPTION "Qt${QT_MAJOR_VERSION} GUI module"
        URL "https://www.qt.io/download")
    # https://bugreports.qt.io/browse/QTBUG-114706
    add_library(Qt::Gui ALIAS Qt${QT_MAJOR_VERSION}::Gui)

    find_package(Qt${QT_MAJOR_VERSION}Widgets ${REQUIRED_QT_VERSION})
    set_package_properties(Qt${QT_MAJOR_VERSION}Widgets PROPERTIES
        TYPE REQUIRED
        DESCRIPTION "Qt${QT_MAJOR_VERSION} widgets module"
        URL "https://www.qt.io/download")
    # https://bugreports.qt.io/browse/QTBUG-114706
    add_library(Qt::Widgets ALIAS Qt${QT_MAJOR_VERSION}::Widgets)

    if(PHONON_BUILD_DESIGNER_PLUGIN)
        # Designer Plugin
        find_package(Qt${QT_MAJOR_VERSION}Designer ${REQUIRED_QT_VERSION})
        set_package_properties(Qt${QT_MAJOR_VERSION}Designer PROPERTIES
            TYPE RECOMMENDED
            DESCRIPTION "Needed to build the Qt Designer plugin"
            URL "https://www.qt.io/download")
        # https://bugreports.qt.io/browse/QTBUG-114706
        if(Qt${QT_MAJOR_VERSION}Xml_FOUND)
            add_library(Qt::Xml ALIAS Qt${QT_MAJOR_VERSION}::Xml)
        endif()
        if(QT_MAJOR_VERSION STREQUAL "6")
            # Qt6Designer links to OpenGLWidgets but doesnt find it
            find_package(Qt6OpenGLWidgets ${REQUIRED_QT_VERSION})
            # https://bugreports.qt.io/browse/QTBUG-114706
            add_library(Qt::OpenGLWidgets ALIAS Qt6::OpenGLWidgets)
        endif()
    endif()

    if(QT_MAJOR_VERSION STREQUAL "6")
        find_package(Qt6Core5Compat REQUIRED)
    endif()

    set(BUILDSYSTEM_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PHONON_LIB_SONAME}/buildsystem/)
    add_definitions(
        -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00
        -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    )

    if(Qt${QT_MAJOR_VERSION}Designer_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
        add_subdirectory(designer designer${QT_MAJOR_VERSION})
    endif()

    add_subdirectory(includes includes${PHONON_LIB_SONAME})
    add_subdirectory(phonon ${PHONON_LIB_SONAME})

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

    install(EXPORT PhononLibs${QT_MAJOR_VERSION}
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

        install(EXPORT PhononExperimentalLibs${QT_MAJOR_VERSION}
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

    if(QT_MAJOR_VERSION VERSION_EQUAL 5)
        ecm_generate_pri_file(
            BASE_NAME ${PHONON_LIB_SONAME}
            LIB_NAME ${PHONON_LIB_SONAME}
            DEPS "core widgets"
            FILENAME_VAR pri_filename
            INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}
        )
        install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
    endif()

    unset(QUERY_EXECUTABLE CACHE)
endforeach()

if(PHONON_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(PHONON_BUILD_DEMOS)
    message(STATUS "Building demos.")
    # Demos may also be built standalone, so we need to skip their find_package call!
    set(CMAKE_DISABLE_FIND_PACKAGE_Phonon4Qt5 TRUE)
    add_subdirectory(demos)
endif()

if(PHONON_BUILD_SETTINGS)
    add_subdirectory(settings)
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/poqm")
    ecm_install_po_files_as_qm(poqm)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
