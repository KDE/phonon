# Defines marcos:
#   TODO doc
#
# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010-2015, Harald Sitter <sitter@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# CMake Defaults

get_filename_component(phonon_cmake_module_dir ${CMAKE_CURRENT_LIST_FILE} PATH)


# Imported from KDE4Defaults.cmake
# Keep this portion copy'n'pastable for updatability.
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} in about every subdir
# since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones
# since cmake 2.4.1
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

#-------------------------------------------------------------------------------

# Include Additional Magic

include(${phonon_cmake_module_dir}/PhononMacros.cmake)

include(${phonon_cmake_module_dir}/MacroLogFeature.cmake)
include(${phonon_cmake_module_dir}/MacroOptionalFindPackage.cmake)

include(CheckCXXCompilerFlag)
include(${phonon_cmake_module_dir}/MacroEnsureVersion.cmake)

# Set Installation Directories - TODO, port to ECM's KDEInstallDirs!

include(GNUInstallDirs)

set(SHARE_INSTALL_PREFIX        "${CMAKE_INSTALL_FULL_DATAROOTDIR}")  #              CACHE PATH "Base directory for files which go to share/")
set(INCLUDE_INSTALL_DIR         "include" ) #           CACHE PATH "The subdirectory to the header prefix")
if (PHONON_BUILD_PHONON4QT5)
    set(INCLUDE_INSTALL_DIR         "include/${PHONON_LIB_SONAME}" ) #           CACHE PATH "The subdirectory to the header prefix")
endif (PHONON_BUILD_PHONON4QT5)
set(BIN_INSTALL_DIR             "bin"     ) #         CACHE PATH "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
set(LIB_INSTALL_DIR             "${CMAKE_INSTALL_LIBDIR}" ) #  CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed"
set(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/kde4"                   CACHE PATH "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
if (PHONON_BUILD_PHONON4QT5)
    set(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/qt5"                   CACHE PATH "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/qt5)" FORCE)
endif(PHONON_BUILD_PHONON4QT5)
set(BACKEND_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/plugins/${PHONON_LIB_SONAME}_backend")
if(WIN32) # Imported from Phonon VLC
    set(BACKEND_INSTALL_DIR "bin/${PHONON_LIB_SONAME}_backend")
endif()
set(ICON_INSTALL_DIR            "${SHARE_INSTALL_PREFIX}/icons"             CACHE PATH "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
set(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde4/services"     CACHE PATH "The install dir for service (desktop, protocol, ...) files")
if (PHONON_BUILD_PHONON4QT5)
    set(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde5/services"     CACHE PATH "The install dir for service (desktop, protocol, ...) files")
endif(PHONON_BUILD_PHONON4QT5)
set(DBUS_INTERFACES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" CACHE PATH "The dbus interfaces install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
set(DBUS_SERVICES_INSTALL_DIR   "${SHARE_INSTALL_PREFIX}/dbus-1/services"   CACHE PATH "The dbus services install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                 LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                 ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel)

# on the Mac support an extra install directory for application bundles
if(APPLE)
    set(INSTALL_TARGETS_DEFAULT_ARGS ${INSTALL_TARGETS_DEFAULT_ARGS}
                                     BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}")
    set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS   "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS}   -flat_namespace -undefined dynamic_lookup")
    set(CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS} -flat_namespace -undefined dynamic_lookup")

   set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
endif(APPLE)

if (NOT PHONON_BUILD_PHONON4QT5) # Qt4
    include(${phonon_cmake_module_dir}/PhononQt4.cmake)
else (NOT PHONON_BUILD_PHONON4QT5) # Qt5
    include(${phonon_cmake_module_dir}/PhononQt5.cmake)
endif (NOT PHONON_BUILD_PHONON4QT5)

# - Automoc (using builtin introduced in 2.8.5)
# NOTE: the compatiibility macros are actively used by the backends, so they
#       cannot be dropped unless the backends get a major release removing all
#       use of them first.

message(STATUS "Using CMake automoc builtin")
set(CMAKE_AUTOMOC TRUE)
# Compatiblity Macros for old automoc nonesense
macro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
    add_executable(${_target_NAME} ${ARGN})
endmacro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
macro(AUTOMOC4_ADD_LIBRARY _target_NAME _add_executable_param)
    add_library(${_target_NAME} ${_add_executable_param} ${ARGN})
endmacro(AUTOMOC4_ADD_LIBRARY)
