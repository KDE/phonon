project(Phonon5)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(PHONON_LIB_SONAME phonon5)
set(PHONON_LIB_SONAME_CAMEL Phonon5)

################################# Requirements #################################

include(FeatureSummary)
include(GNUInstallDirs)
#include(GenerateExportHeader)

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)

find_package(Qt5 5.2.0 CONFIG REQUIRED Core Gui Quick Widgets)
find_package(OpenGL REQUIRED)

ecm_setup_version("4.50.0"
    VARIABLE_PREFIX PHONON
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/phononversiondata.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake"
    SOVERSION 4)

################################### Options ####################################

option(PHONON_BUILD_DECLARATIVE_PLUGIN "Build the Qt Declarative (QML) plugin" OFF)
option(PHONON_BUILD_DESIGNER_PLUGIN "Build the Qt Designer plugin" OFF)
option(PHONON_BUILD_DOC "Build the API documentation" OFF)

################################################################################

# Using relative pathery everywhere is crucial for cmake's targets file to
# contain an include directory definition for the target, which in turn makes
# Phonon much easier to use when using cmake to build the consumer.
message(WARNING "includes definition is partially duplicated in actual install command")
set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION  "${CMAKE_INSTALL_BINDIR}"
                                  LIBRARY DESTINATION  "${CMAKE_INSTALL_LIBDIR}"
                                  ARCHIVE DESTINATION  "${CMAKE_INSTALL_LIBDIR}" COMPONENT Devel
                                  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PHONON_LIB_SONAME}"
)
message(WARNING "plugininstalldir is possibly a bit fugly")
set(PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/plugins/")

message(WARNING "phonon buildsystem currently not used")
#add_subdirectory(cmake)
if(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif(QT_QTDESIGNER_FOUND AND PHONON_BUILD_DESIGNER_PLUGIN)

if(QT_QTDECLARATIVE_FOUND AND PHONON_BUILD_DECLARATIVE_PLUGIN)
    add_subdirectory(declarative)
endif(QT_QTDECLARATIVE_FOUND AND PHONON_BUILD_DECLARATIVE_PLUGIN)

if(PHONON_BUILD_DOC)
    add_subdirectory(doc)
endif(PHONON_BUILD_DOC)

add_subdirectory(src)
#add_subdirectory(includes)

################################# CMake Config #################################

set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PHONON_LIB_SONAME})

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/PhononConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME_CAMEL}ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(
    EXPORT PhononTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE PhononTargets.cmake
    NAMESPACE Phonon::
)

################################# PKG Config #################################

if(NOT WIN32)
    configure_file(phonon.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PHONON_LIB_SONAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

################################### QT PRI #####################################

message(WARNING "ecm_generate_pri uses kf5 include path...")
ecm_generate_pri_file(BASE_NAME ${PHONON_LIB_SONAME} LIB_NAME ${PHONON_LIB_SONAME_CAMEL} DEPS "widgets" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

################################################################################

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
