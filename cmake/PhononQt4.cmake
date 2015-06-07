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

# RPATH Handling

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
set(_abs_LIB_INSTALL_DIR "${LIB_INSTALL_DIR}")
if (NOT IS_ABSOLUTE "${_abs_LIB_INSTALL_DIR}")
   set(_abs_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
endif()
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${_abs_LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${_abs_LIB_INSTALL_DIR}" _isSystemCxxLibDir)
if("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${_abs_LIB_INSTALL_DIR}")
endif("${_isSystemPlatformLibDir}" STREQUAL "-1" AND "${_isSystemCxxLibDir}" STREQUAL "-1")

# Find Qt4

# Store CMAKE_MODULE_PATH and then append the current dir to it, so we are sure
# we get the FindQt4.cmake located next to us and not a different one.
# The original CMAKE_MODULE_PATH is restored later on.
set(_phonon_cmake_module_path_back ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${phonon_cmake_module_dir} ${CMAKE_MODULE_PATH} )

if (NOT QT_MIN_VERSION)
    set(QT_MIN_VERSION "4.6.0")
endif (NOT QT_MIN_VERSION)
if (${QT_MIN_VERSION} VERSION_LESS "4.6.0")
    set(QT_MIN_VERSION "4.6.0")
endif (${QT_MIN_VERSION} VERSION_LESS "4.6.0")

find_package(Qt4)
macro_log_feature(QT4_FOUND "Qt4" "" "" TRUE)

# ----- compat
macro (qt5_use_modules target)
    set(_deps "")
    foreach (arg ${ARGN})
        if (arg STREQUAL "Core")
            list(APPEND _deps ${QT_QTCORE_LIBRARY})
        elseif (arg STREQUAL "Gui")
            list(APPEND _deps ${QT_QTGUI_LIBRARY})
        elseif (arg STREQUAL "Widgets")
            list(APPEND _deps ${QT_QTGUI_LIBRARY})
        elseif (arg STREQUAL "DBus")
            list(APPEND _deps ${QT_QTDBUS_LIBRARY})
        elseif (arg STREQUAL "OpenGL")
            list(APPEND _deps ${QT_QTOPENGL_LIBRARY})
        elseif (arg STREQUAL "Declarative")
            list(APPEND _deps ${QT_QTDECLARATIVE_LIBRARY})
        elseif (arg STREQUAL "Designer")
            list(APPEND _deps ${QT_QTDESIGNER_LIBRARY})
        else ()
            message("qt5_use_modules could not map ${arg} to Qt 4")
        endif ()
    endforeach ()
    target_link_libraries(${target} ${_deps})
endmacro (qt5_use_modules target args)

macro (qt5_add_resources)
    qt4_add_resources(${ARGN})
endmacro (qt5_add_resources)

macro (qt5_wrap_ui)
    qt4_wrap_ui(${ARGN})
endmacro ()


# restore the original CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${_phonon_cmake_module_path_back})

# Imported from FindKDE4Internal.cmake
# Keep this portion copy'n'pastable for updatability.
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
   if (CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
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
   endif (CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
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

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES Clang)
   _DETERMINE_GCC_SYSTEM_INCLUDE_DIRS(c _dirs)
   set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES
       ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES} ${_dirs})
endif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES Clang)

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
      set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols")
      set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--export-all-symbols")
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
      set(_include_dirs "-DINCLUDE_DIRECTORIES:STRING=${QT_INCLUDES}")

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


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   _DETERMINE_GCC_SYSTEM_INCLUDE_DIRS(c++ _dirs)
   set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES
       ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES} ${_dirs})

   # Note that exceptions are enabled by default when building with clang. That
   # is, -fno-exceptions is not set in CMAKE_CXX_FLAGS below. This is because a
   # lot of code in different KDE modules ends up including code that throws
   # exceptions. Most (or all) of the occurrences are in template code that
   # never gets instantiated. Contrary to GCC, ICC and MSVC, clang (most likely
   # rightfully) complains about that. Trying to work around the issue by
   # passing -fdelayed-template-parsing brings other problems, as noted in
   # http://lists.kde.org/?l=kde-core-devel&m=138157459706783&w=2.
   # The generated code will be slightly bigger, but there is no way to avoid
   # it.
   set(KDE4_ENABLE_EXCEPTIONS "-fexceptions -UQT_NO_EXCEPTIONS")

   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-inline")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline")
   set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-inline")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline")
   set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -Woverloaded-virtual -fno-common -fvisibility=hidden -Werror=return-type -fvisibility-inlines-hidden")
   set(KDE4_C_FLAGS    "-fvisibility=hidden")

   # At least kdepim exports one function with C linkage that returns a
   # QString in a plugin, but clang does not like that.
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage")

   set(KDE4_CXX_FPIE_FLAGS "-fPIE")
   set(KDE4_PIE_LDFLAGS    "-pie")

   if (CMAKE_SYSTEM_NAME STREQUAL GNU)
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
      set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -pthread")
   endif (CMAKE_SYSTEM_NAME STREQUAL GNU)

   set(__KDE_HAVE_GCC_VISIBILITY TRUE)

   # check that Qt defines Q_DECL_EXPORT as __attribute__ ((visibility("default")))
   # if it doesn't and KDE compiles with hidden default visibiltiy plugins will break
   set(_source "#include <QtCore/QtGlobal>\n int main()\n {\n #ifndef QT_VISIBILITY_AVAILABLE \n #error QT_VISIBILITY_AVAILABLE is not available\n #endif \n }\n")
   set(_source_file ${CMAKE_BINARY_DIR}/CMakeTmp/check_qt_visibility.cpp)
   file(WRITE "${_source_file}" "${_source}")
   set(_include_dirs "-DINCLUDE_DIRECTORIES:STRING=${QT_INCLUDES}")
   try_compile(_compile_result ${CMAKE_BINARY_DIR} ${_source_file} CMAKE_FLAGS "${_include_dirs}" OUTPUT_VARIABLE _compile_output_var)
   if(NOT _compile_result)
       message("${_compile_output_var}")
       message(FATAL_ERROR "Qt compiled without support for -fvisibility=hidden. This will break plugins and linking of some applications. Please fix your Qt installation (try passing --reduce-exports to configure).")
   endif(NOT _compile_result)
endif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")


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

# Uninstall Target
if (NOT _phonon_uninstall_target_created)
   set(_phonon_uninstall_target_created TRUE)
   configure_file("${phonon_cmake_module_dir}/cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
   add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")
endif (NOT _phonon_uninstall_target_created)
