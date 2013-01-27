# Defines marcos:
#   TODO doc
#
# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010, Harald Sitter <sitter@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

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


# Check Requirements

# - Qt

# Store CMAKE_MODULE_PATH and then append the current dir to it, so we are sure
# we get the FindQt4.cmake located next to us and not a different one.
# The original CMAKE_MODULE_PATH is restored later on.
set(_phonon_cmake_module_path_back ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${phonon_cmake_module_dir} ${CMAKE_MODULE_PATH} )

# # Tell FindQt4.cmake to point the QT_QTFOO_LIBRARY targets at the imported targets
# # for the Qt libraries, so we get full handling of release and debug versions of the
# # Qt libs and are flexible regarding the install location of Qt under Windows:
# set(QT_USE_IMPORTED_TARGETS TRUE)

find_package(Qt5Core REQUIRED)
macro_log_feature(Qt5Core_FOUND "Qt5 Core (qtbase)" "" "" TRUE)

find_package(Qt5Gui REQUIRED)
macro_log_feature(Qt5Gui_FOUND "Qt5 Gui (qtbase)" "" "" TRUE)

find_package(Qt5Widgets REQUIRED)
macro_log_feature(Qt5Widgets_FOUND "Qt5 Widgets (qtbase)" "" "" TRUE)

if (Qt5_POSITION_INDEPENDENT_CODE)
   set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Compat variables for plugins.
function(_QT4_QUERY_QMAKE VAR RESULT)
    get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} LOCATION)
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} "-query" ${VAR}
                    RESULT_VARIABLE return_code
                    OUTPUT_VARIABLE output)
    if(NOT return_code)
        file(TO_CMAKE_PATH "${output}" output)
        STRING(REGEX REPLACE "(\r?\n)+$" "" output "${output}")
        set(${RESULT} ${output} PARENT_SCOPE)
    endif(NOT return_code)
endfunction(_QT4_QUERY_QMAKE)

_qt4_query_qmake(QT_INSTALL_IMPORTS QT_IMPORTS_DIR)
_qt4_query_qmake(QT_HOST_DATA QT_MKSPECS_DIR)
_qt4_query_qmake(QT_INSTALL_PLUGINS QT_PLUGINS_DIR)

set(QT_MKSPECS_DIR "${QT_MKSPECS_DIR}/mkspecs")

# - Automoc

# Starting with CMake 2.8.6 there is a builtin to replace automoc4, use that when possible.
if(CMAKE_VERSION VERSION_GREATER 2.8.5)
    message(STATUS "Using CMake automoc builtin")
    set(CMAKE_AUTOMOC TRUE)
    # Compatiblity Macros
    macro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
        add_executable(${_target_NAME} ${ARGN})
    endmacro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
    macro(AUTOMOC4_ADD_LIBRARY _target_NAME _add_executable_param)
        add_library(${_target_NAME} ${_add_executable_param} ${ARGN})
    endmacro(AUTOMOC4_ADD_LIBRARY)
else(CMAKE_VERSION VERSION_GREATER 2.8.5)
    message(STATUS "Can not use CMake automoc builtin, trying to find automoc4")
    find_package(Automoc4 REQUIRED)
    if (NOT AUTOMOC4_VERSION)
        set(AUTOMOC4_VERSION "0.9.83")
    endif (NOT AUTOMOC4_VERSION)
    macro_ensure_version("0.9.86" "${AUTOMOC4_VERSION}" _automoc4_version_ok)
    if (NOT _automoc4_version_ok)
        message(FATAL_ERROR "Your version of automoc4 is too old. You have ${AUTOMOC4_VERSION}, you need at least 0.9.86")
    endif (NOT _automoc4_version_ok)
endif(CMAKE_VERSION VERSION_GREATER 2.8.5)

# restore the original CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${_phonon_cmake_module_path_back})

# Set Installation Directories - TODO, port to ECM's KDEInstallDirs!

include(GNUInstallDirs)

set(SHARE_INSTALL_PREFIX        "share")  #              CACHE PATH "Base directory for files which go to share/")
set(INCLUDE_INSTALL_DIR         "include/phonon4qt5" ) #           CACHE PATH "The subdirectory to the header prefix")
set(BIN_INSTALL_DIR             "bin"     ) #         CACHE PATH "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
set(LIB_INSTALL_DIR             "${CMAKE_INSTALL_LIBDIR}" ) #  CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed"
set(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/qt5"                   CACHE PATH "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
set(ICON_INSTALL_DIR            "${SHARE_INSTALL_PREFIX}/icons"             CACHE PATH "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
set(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde5/services"     CACHE PATH "The install dir for service (desktop, protocol, ...) files")
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

# RPATH Handling

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library 
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")
endif("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")


# Uninstall Target
if (NOT _phonon_uninstall_target_created)
   set(_phonon_uninstall_target_created TRUE)
   configure_file("${phonon_cmake_module_dir}/cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
   add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")
endif (NOT _phonon_uninstall_target_created)


# Imported from FindKDE4Internal.cmake
# Keep this portion copy'n'pastable for updatability.
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
   if (CMAKE_COMPILER_IS_GNUCXX)
      set ( _KDE4_PLATFORM_DEFINITIONS -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_GNU_SOURCE)
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")

      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_MODULE_LINKER_FLAGS}")
      set ( CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_EXE_LINKER_FLAGS}")

      # we profile...
      if(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
      endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
   endif (CMAKE_COMPILER_IS_GNUCXX)
   if (CMAKE_C_COMPILER MATCHES "icc")
      set ( _KDE4_PLATFORM_DEFINITIONS -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_GNU_SOURCE)
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
   endif (CMAKE_C_COMPILER MATCHES "icc")
endif (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)

#-------------------------------------------------------------------------------


# Imported from FindKDE4Internal.cmake
# Keep this portion copy'n'pastable for updatability.
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# this macro is for internal use only.
macro(KDE_CHECK_FLAG_EXISTS FLAG VAR DOC)
   if(NOT ${VAR} MATCHES "${FLAG}")
      set(${VAR} "${${VAR}} ${FLAG}" CACHE STRING "Flags used by the linker during ${DOC} builds." FORCE)
   endif(NOT ${VAR} MATCHES "${FLAG}")
endmacro(KDE_CHECK_FLAG_EXISTS FLAG VAR)

if (MSVC)
   set (KDE4_ENABLE_EXCEPTIONS -EHsc)

   # Qt disables the native wchar_t type, do it too to avoid linking issues
   set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zc:wchar_t-" )

   # make sure that no header adds libcmt by default using #pragma comment(lib, "libcmt.lib") as done by mfc/afx.h
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "Release with Debug Info")
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_RELEASE "release")
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "release minsize")
   kde_check_flag_exists("/NODEFAULTLIB:libcmtd /DEFAULTLIB:msvcrtd" CMAKE_EXE_LINKER_FLAGS_DEBUG "debug")
endif(MSVC)

# This macro is for internal use only
# Return the directories present in gcc's include path.
macro(_DETERMINE_GCC_SYSTEM_INCLUDE_DIRS _lang _result)
  set(${_result})
  set(_gccOutput)
  file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy" "\n" )
  execute_process(COMMAND ${CMAKE_C_COMPILER} -v -E -x ${_lang} -dD dummy
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles
                  ERROR_VARIABLE _gccOutput
                  OUTPUT_VARIABLE _gccStdout )
  file(REMOVE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy")

  if( "${_gccOutput}" MATCHES "> search starts here[^\n]+\n *(.+) *\n *End of (search) list" )
    SET(${_result} ${CMAKE_MATCH_1})
    STRING(REPLACE "\n" " " ${_result} "${${_result}}")
    SEPARATE_ARGUMENTS(${_result})
  ENDIF( "${_gccOutput}" MATCHES "> search starts here[^\n]+\n *(.+) *\n *End of (search) list" )
ENDMACRO(_DETERMINE_GCC_SYSTEM_INCLUDE_DIRS _lang)

if (CMAKE_COMPILER_IS_GNUCC)
   _DETERMINE_GCC_SYSTEM_INCLUDE_DIRS(c _dirs)
   set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES
       ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES} ${_dirs})
endif (CMAKE_COMPILER_IS_GNUCC)

if (CMAKE_COMPILER_IS_GNUCXX)
   _DETERMINE_GCC_SYSTEM_INCLUDE_DIRS(c++ _dirs)
   set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
       ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES} ${_dirs})

   set (KDE4_ENABLE_EXCEPTIONS "-fexceptions -UQT_NO_EXCEPTIONS")
   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline")
   set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline")
   set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
   # As of Qt 4.6.x we need to override the new exception macros if we want compile with -fno-exceptions
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-check-new -fno-common")

   if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
     # This should not be needed, as it is also part of _KDE4_PLATFORM_DEFINITIONS below.
     # It is kept here nonetheless both for backwards compatibility in case one does not use add_definitions(${KDE4_DEFINITIONS})
     # and also because it is/was needed by glibc for snprintf to be available when building C files.
     # See commit 4a44862b2d178c1d2e1eb4da90010d19a1e4a42c.
     add_definitions (-D_BSD_SOURCE)
   endif (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)

   if (CMAKE_SYSTEM_NAME STREQUAL GNU)
      set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
      set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -pthread")
   endif (CMAKE_SYSTEM_NAME STREQUAL GNU)

   # gcc under Windows
   if (MINGW)
      set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols -Wl,--disable-auto-import")
      set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--export-all-symbols -Wl,--disable-auto-import")
   endif (MINGW)

   check_cxx_compiler_flag(-fPIE HAVE_FPIE_SUPPORT)
   if(KDE4_ENABLE_FPIE)
       if(HAVE_FPIE_SUPPORT)
        set (KDE4_CXX_FPIE_FLAGS "-fPIE")
        set (KDE4_PIE_LDFLAGS "-pie")
       else(HAVE_FPIE_SUPPORT)
        message(STATUS "Your compiler doesn't support the PIE flag")
       endif(HAVE_FPIE_SUPPORT)
   endif(KDE4_ENABLE_FPIE)

   check_cxx_compiler_flag(-Woverloaded-virtual __KDE_HAVE_W_OVERLOADED_VIRTUAL)
   if(__KDE_HAVE_W_OVERLOADED_VIRTUAL)
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
   endif(__KDE_HAVE_W_OVERLOADED_VIRTUAL)

   # visibility support
   check_cxx_compiler_flag(-fvisibility=hidden __KDE_HAVE_GCC_VISIBILITY)
   set( __KDE_HAVE_GCC_VISIBILITY ${__KDE_HAVE_GCC_VISIBILITY} CACHE BOOL "GCC support for hidden visibility")

   # get the gcc version
   exec_program(${CMAKE_C_COMPILER} ARGS ${CMAKE_C_COMPILER_ARG1} --version OUTPUT_VARIABLE _gcc_version_info)

   string (REGEX MATCH "[345]\\.[0-9]\\.[0-9]" _gcc_version "${_gcc_version_info}")
   # gcc on mac just reports: "gcc (GCC) 3.3 20030304 ..." without the patch level, handle this here:
   if (NOT _gcc_version)
      string (REGEX MATCH ".*\\(GCC\\).* ([34]\\.[0-9]) .*" "\\1.0" _gcc_version "${gcc_on_macos}")
      if (gcc_on_macos)
        string (REGEX REPLACE ".*\\(GCC\\).* ([34]\\.[0-9]) .*" "\\1.0" _gcc_version "${_gcc_version_info}")
      endif (gcc_on_macos)
   endif (NOT _gcc_version)

   if (_gcc_version)
      macro_ensure_version("4.1.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_1)
      macro_ensure_version("4.2.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_2)
      macro_ensure_version("4.3.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_3)
   endif (_gcc_version)

   # save a little by making local statics not threadsafe
   # ### do not enable it for older compilers, see
   # ### http://gcc.gnu.org/bugzilla/show_bug.cgi?id=31806
   if (GCC_IS_NEWER_THAN_4_3)
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")
   endif (GCC_IS_NEWER_THAN_4_3)

   set(_GCC_COMPILED_WITH_BAD_ALLOCATOR FALSE)
   if (GCC_IS_NEWER_THAN_4_1)
      exec_program(${CMAKE_C_COMPILER} ARGS ${CMAKE_C_COMPILER_ARG1} -v OUTPUT_VARIABLE _gcc_alloc_info)
      string(REGEX MATCH "(--enable-libstdcxx-allocator=mt)" _GCC_COMPILED_WITH_BAD_ALLOCATOR "${_gcc_alloc_info}")
   endif (GCC_IS_NEWER_THAN_4_1)

   if (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR AND NOT WIN32)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
      set (KDE4_C_FLAGS "-fvisibility=hidden")
      # check that Qt defines Q_DECL_EXPORT as __attribute__ ((visibility("default")))
      # if it doesn't and KDE compiles with hidden default visibiltiy plugins will break
      set(_source "#include <QtCore/QtGlobal>\n int main()\n {\n #ifndef QT_VISIBILITY_AVAILABLE \n #error QT_VISIBILITY_AVAILABLE is not available\n #endif \n }\n")
      set(_source_file ${CMAKE_BINARY_DIR}/CMakeTmp/check_qt_visibility.cpp)
      file(WRITE "${_source_file}" "${_source}")
      set(_include_dirs "-DINCLUDE_DIRECTORIES:STRING=${Qt5Core_INCLUDE_DIRS}")

      try_compile(_compile_result ${CMAKE_BINARY_DIR} ${_source_file} CMAKE_FLAGS "${_include_dirs}" OUTPUT_VARIABLE _compile_output_var)

      if(NOT _compile_result)
         message("${_compile_output_var}")
         message(FATAL_ERROR "Qt compiled without support for -fvisibility=hidden. This will break plugins and linking of some applications. Please fix your Qt installation (try passing --reduce-exports to configure).")
      endif(NOT _compile_result)

      if (GCC_IS_NEWER_THAN_4_2)
         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -fvisibility-inlines-hidden")
      endif (GCC_IS_NEWER_THAN_4_2)
   else (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR AND NOT WIN32)
      set (__KDE_HAVE_GCC_VISIBILITY 0)
   endif (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR AND NOT WIN32)

endif (CMAKE_COMPILER_IS_GNUCXX)


if (CMAKE_C_COMPILER MATCHES "icc")

   set (KDE4_ENABLE_EXCEPTIONS -fexceptions)
   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_DEBUG          "-O2 -g -fno-inline -noalign")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g -fno-inline -noalign")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
   set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_DEBUG            "-O2 -g -fno-inline -noalign")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g -fno-inline -noalign")

   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -ansi -Wall -w1 -Wpointer-arith -fno-common")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -Wall -w1 -Wpointer-arith -fno-exceptions -fno-common")

   # visibility support
   set(__KDE_HAVE_ICC_VISIBILITY)
#   check_cxx_compiler_flag(-fvisibility=hidden __KDE_HAVE_ICC_VISIBILITY)
#   if (__KDE_HAVE_ICC_VISIBILITY)
#      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
#   endif (__KDE_HAVE_ICC_VISIBILITY)

endif (CMAKE_C_COMPILER MATCHES "icc")

#-------------------------------------------------------------------------------


# Random Stuff

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
   set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")

# For Windows
if(MSVC)
    if(CMAKE_COMPILER_2005)
        # to avoid a lot of deprecated warnings
        add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
    endif(CMAKE_COMPILER_2005)
endif(MSVC)
