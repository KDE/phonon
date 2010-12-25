# Defines marcos:
#   TODO doc
#
# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010, Harald Sitter <sitter@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# Find Phonon

macro(_phonon_find_version)
   if (APPLE AND EXISTS "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")
      set(_phonon_namespace_header_file "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")
      set(_phonon_pulsesupport_header_file "${PHONON_INCLUDE_DIR}/Headers/pulsesupport.h")
   else ()
      set(_phonon_namespace_header_file "${PHONON_INCLUDE_DIR}/phonon/phononnamespace.h")
      set(_phonon_pulsesupport_header_file "${PHONON_INCLUDE_DIR}/phonon/pulsesupport.h")
   endif (APPLE AND EXISTS "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")

   file(READ ${_phonon_namespace_header_file} _phonon_header LIMIT 5000 OFFSET 1000)
   string(REGEX MATCH "define PHONON_VERSION_STR \"(4\\.[0-9]+\\.[0-9a-z]+)\"" _phonon_version_match "${_phonon_header}")
   set(PHONON_VERSION "${CMAKE_MATCH_1}")
   message(STATUS "Phonon Version: ${PHONON_VERSION}")
endmacro(_phonon_find_version)

if(PHONON_FOUND)
   # Already found, nothing more to do except figuring out the version
   _phonon_find_version()
else(PHONON_FOUND)
   if(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
      set(PHONON_FIND_QUIETLY TRUE)
   endif(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)

   # As discussed on kde-buildsystem: first look at CMAKE_PREFIX_PATH, then at the suggested PATHS (kde4 install dir)
   find_library(PHONON_LIBRARY NAMES phonon PATHS ${KDE4_LIB_INSTALL_DIR} ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
   # then at the default system locations (CMAKE_SYSTEM_PREFIX_PATH, i.e. /usr etc.)
   find_library(PHONON_LIBRARY NAMES phonon)

   find_library(PHONON_LIBRARY_EXPERIMENTAL NAMES phononexperimental PATHS ${KDE4_LIB_INSTALL_DIR} ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
   find_library(PHONON_LIBRARY_EXPERIMENTAL phononexperimental)

   find_path(PHONON_INCLUDE_DIR NAMES phonon/phonon_export.h PATHS ${KDE4_INCLUDE_INSTALL_DIR} ${QT_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR} ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
   find_path(PHONON_INCLUDE_DIR NAMES phonon/phonon_export.h)

   find_path(PHONON_INCLUDE_DIR_EXPERIMENTAL NAMES phonon/experimental/export.h PATHS ${KDE4_INCLUDE_INSTALL_DIR} ${QT_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR} ${QT_LIBRARY_DIR} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
   find_path(PHONON_INCLUDE_DIR_EXPERIMENTAL NAMES phonon/experimental/export.h)

   if(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
      set(PHONON_LIBS ${phonon_LIB_DEPENDS} ${PHONON_LIBRARY})
      set(PHONON_INCLUDES ${PHONON_INCLUDE_DIR}/KDE ${PHONON_INCLUDE_DIR})
      set(PHONON_FOUND TRUE)
      _phonon_find_version()
      find_path(PHONON_PULSESUPPORT NAMES phonon/pulsesupport.h PATHS ${PHONON_INCLUDES})

      if(PHONON_PULSESUPPORT)
         # Check if the method enable() is available in pulsesupport.h
         file(READ ${_phonon_pulsesupport_header_file} _pulsesupport_header)
         string(REGEX MATCH "void enable" _phonon_pulse_match "${_pulsesupport_header}")
         set(PHONON_PULSESUPPORT "${_phonon_pulse_match}")
      endif(PHONON_PULSESUPPORT)

      if(PHONON_INCLUDE_DIR_EXPERIMENTAL AND PHONON_LIBRARY_EXPERIMENTAL)
         set(PHONON_LIBS ${PHONON_LIBS} ${PHONON_LIBRARY_EXPERIMENTAL})
         set(PHONON_INCLUDES ${PHONON_INCLUDES} ${PHONON_INCLUDE_DIR_EXPERIMENTAL})
         set(PHONON_FOUND_EXPERIMENTAL TRUE)
         message(STATUS "Found Phonon Experimental: ${PHONON_LIBRARY_EXPERIMENTAL}")
      endif(PHONON_INCLUDE_DIR_EXPERIMENTAL AND PHONON_LIBRARY_EXPERIMENTAL)

   else(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
      set(PHONON_FOUND FALSE)
   endif(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)

   if(PHONON_FOUND)
      if(NOT PHONON_FIND_QUIETLY)
         message(STATUS "Found Phonon: ${PHONON_LIBRARY}")
         message(STATUS "Found Phonon Includes: ${PHONON_INCLUDES}")
         if(PHONON_PULSESUPPORT)
            message(STATUS "Found Phonon PulseAudio Support: Yes")
         else(PHONON_PULSESUPPORT)
            message(STATUS "Found Phonon PulseAudio Support: No")
         endif(PHONON_PULSESUPPORT)
      endif(NOT PHONON_FIND_QUIETLY)
   else(PHONON_FOUND)
      if(Phonon_FIND_REQUIRED)
         if(NOT PHONON_INCLUDE_DIR)
            message(STATUS "Phonon includes NOT found!")
         endif(NOT PHONON_INCLUDE_DIR)
         if(NOT PHONON_LIBRARY)
            message(STATUS "Phonon library NOT found!")
         endif(NOT PHONON_LIBRARY)
         message(FATAL_ERROR "Phonon library or includes NOT found!")
      else(Phonon_FIND_REQUIRED)
         message(STATUS "Unable to find Phonon")
      endif(Phonon_FIND_REQUIRED)
   endif(PHONON_FOUND)

   mark_as_advanced(
        PHONON_INCLUDE_DIR
        PHONON_INCLUDE_DIR_EXPERIMENTAL
        PHONON_LIBRARY
        PHONON_LIBRARY_EXPERIMENTAL
        PHONON_INCLUDES
   )
endif(PHONON_FOUND)


# Include Additional Magic

include(PhononMacros)

include(MacroLogFeature)
include(MacroOptionalFindPackage)

include(CheckCXXCompilerFlag)
include(MacroEnsureVersion)


# Check Requirements

# - Qt

# if the minimum Qt requirement is changed, change all occurrence in the
# following lines
if (NOT QT_MIN_VERSION)
  set(QT_MIN_VERSION "4.6.0")
endif (NOT QT_MIN_VERSION)
if (${QT_MIN_VERSION} VERSION_LESS "4.6.0")
  set(QT_MIN_VERSION "4.6.0")
endif (${QT_MIN_VERSION} VERSION_LESS "4.6.0")

# # Tell FindQt4.cmake to point the QT_QTFOO_LIBRARY targets at the imported targets
# # for the Qt libraries, so we get full handling of release and debug versions of the
# # Qt libs and are flexible regarding the install location of Qt under Windows:
# set(QT_USE_IMPORTED_TARGETS TRUE)

find_package(Qt4 ${_REQ_STRING_KDE4})

# TODO: do we want this here?
if (NOT QT_QTDBUS_FOUND)
   message(STATUS "Warning: Building without DBus support as QtDBus was not found.")
endif(NOT QT_QTDBUS_FOUND)

# - Automoc

find_package(Automoc4 REQUIRED)
if (NOT AUTOMOC4_VERSION)
    set(AUTOMOC4_VERSION "0.9.83")
endif (NOT AUTOMOC4_VERSION)
macro_ensure_version("0.9.86" "${AUTOMOC4_VERSION}" _automoc4_version_ok)
if (NOT _automoc4_version_ok)
    message(FATAL_ERROR "Your version of automoc4 is too old. You have ${AUTOMOC4_VERSION}, you need at least 0.9.86")
endif (NOT _automoc4_version_ok)


# Set Installation Directories

macro(_SET_FANCY _var _value _comment)
    if (NOT DEFINED ${_var})
        set(${_var} "${_value}")
    else (NOT DEFINED ${_var})
        set(${_var} "${${_var}}" CACHE PATH "${_comment}")
    endif (NOT DEFINED ${_var})
endmacro(_SET_FANCY)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )

if (WIN32)
    _set_fancy(EXEC_INSTALL_PREFIX     "."                                         "Base directory for executables and libraries")
else (WIN32)
    _set_fancy(EXEC_INSTALL_PREFIX     "${CMAKE_INSTALL_PREFIX}"                   "Base directory for executables and libraries")
endif (WIN32)
_set_fancy(SHARE_INSTALL_PREFIX        "${EXEC_INSTALL_PREFIX}/share"              "Base directory for files which go to share/")
_set_fancy(INCLUDE_INSTALL_DIR         "${EXEC_INSTALL_PREFIX}/include"            "The subdirectory to the header prefix")
_set_fancy(BIN_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/bin"                "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
_set_fancy(LIB_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}"   "The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})")
_set_fancy(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/kde4"                   "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
_set_fancy(ICON_INSTALL_DIR            "${SHARE_INSTALL_PREFIX}/icons"             "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
_set_fancy(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde4/services"     "The install dir for service (desktop, protocol, ...) files")
_set_fancy(DBUS_INTERFACES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" "The dbus interfaces install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
_set_fancy(DBUS_SERVICES_INSTALL_DIR   "${SHARE_INSTALL_PREFIX}/dbus-1/services"   "The dbus services install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

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
set(CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")


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

   if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
     set ( CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
     # As off Qt 4.6.x we need to override the new exception macros if we want compile with -fno-exceptions
     set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-check-new -fno-common")
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

      # we always link against the release version of QT with mingw
      # (even for debug builds). So we need to define QT_NO_DEBUG
      # or else QPluginLoader rejects plugins because it thinks
      # they're built against the wrong QT.
      add_definitions(-DQT_NO_DEBUG)
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
      set(_include_dirs "-DINCLUDE_DIRECTORIES:STRING=${QT_INCLUDES}")

      try_compile(_compile_result ${CMAKE_BINARY_DIR} ${_source_file} CMAKE_FLAGS "${_include_dirs}" COMPILE_OUTPUT_VARIABLE _compile_output_var)

      if(NOT _compile_result)
         message(FATAL_ERROR "Qt compiled without support for -fvisibility=hidden. This will break plugins and linking of some applications. Please fix your Qt installation.")
      endif(NOT _compile_result)

      if (GCC_IS_NEWER_THAN_4_2)
          set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
      endif (GCC_IS_NEWER_THAN_4_2)
   else (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR AND NOT WIN32)
      set (__KDE_HAVE_GCC_VISIBILITY 0)
   endif (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR AND NOT WIN32)

endif (CMAKE_COMPILER_IS_GNUCXX)

#-------------------------------------------------------------------------------


# Random Stuff

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
   set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
