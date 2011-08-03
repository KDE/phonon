project(libdavros)

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 COMPONENTS QtCore REQUIRED)
include(${QT_USE_FILE})

set(DAVROS_LIB_MAJOR_VERSION "1") # Only change on binary incompatible changes
set(DAVROS_LIB_MINOR_VERSION "0") # Only change on binary compatible changes with new interfaces
set(DAVROS_LIB_PATCH_VERSION "0") # Bump whenever you feel like it
set(DAVROS_LIB_VERSION "${DAVROS_LIB_MAJOR_VERSION}.${DAVROS_LIB_MINOR_VERSION}.${DAVROS_LIB_PATCH_VERSION}")
set(DAVROS_LIB_SOVERSION ${DAVROS_LIB_MAJOR_VERSION})

set(CMAKECONFIG_INSTALL_DIR lib${LIB_SUFFIX}/cmake/davros)
set(BUILDSYSTEM_INSTALL_DIR share/davros/buildsystem/)

add_subdirectory(cmake)

add_library(davros SHARED davros.cpp block.cpp)
target_link_libraries(davros ${QT_LIBRARIES})
set_target_properties(davros PROPERTIES VERSION ${DAVROS_LIB_VERSION} SOVERSION ${DAVROS_LIB_SOVERSION})

file(RELATIVE_PATH relInstallDir ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DavrosConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/DavrosConfig.cmake @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DavrosConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/DavrosConfigVersion.cmake @ONLY)


install(TARGETS davros
        EXPORT DavrosLib
        DESTINATION lib${LIB_SUFFIX})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DavrosConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/DavrosConfigVersion.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT DavrosLib DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE DavrosTargets.cmake)
install(FILES davros.h block.h
        DESTINATION include/davros)
