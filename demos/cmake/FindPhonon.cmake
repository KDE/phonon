# Find libphonon
# Once done this will define
#
#  PHONON_FOUND       - system has Phonon Library
#  PHONON_INCLUDE_DIR - the Phonon include directory
#  PHONON_LIBRARY     - link these to use Phonon
#  PHONON_VERSION     - the version of the Phonon Library

# Copyright (c) 2010, Harald Sitter <sitter@kde.org>
# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(_phonon_find_version)
    set(_phonon_namespace_header_file "${PHONON_INCLUDE_DIR}/phonon/phononnamespace.h")
    if (APPLE AND EXISTS "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")
        set(_phonon_namespace_header_file "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")
    endif (APPLE AND EXISTS "${PHONON_INCLUDE_DIR}/Headers/phononnamespace.h")
    file(READ ${_phonon_namespace_header_file} _phonon_header LIMIT 5000 OFFSET 1000)
    string(REGEX MATCH "define PHONON_VERSION_STR "(4\\.[0-9]+\\.[0-9a-z]+)"" _phonon_version_match "${_phonon_header}")
    set(PHONON_VERSION "${CMAKE_MATCH_1}")
endmacro(_phonon_find_version)

if(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
    set(PHONON_FIND_QUIETLY TRUE)
else()
   find_package(PkgConfig)
   pkg_check_modules(PC_PHONON phonon)
   set(PHONON_DEFINITIONS ${PC_PHONON_CFLAGS_OTHER})

   find_library(PHONON_LIBRARY NAMES phonon
                   HINTS ${PC_PHONON_LIBDIR} ${PC_PHONON_LIBRARY_DIRS}
                       ${KDE4_LIB_INSTALL_DIR} ${QT_LIBRARY_DIR})

   find_path(PHONON_INCLUDE_DIR NAMES phonon/phonon_export.h
               HINTS ${PC_PHONON_INCLUDEDIR} ${PC_PHONON_INCLUDE_DIRS}
                   ${KDE4_INCLUDE_INSTALL_DIR} ${QT_INCLUDE_DIR}
                   ${INCLUDE_INSTALL_DIR} ${QT_LIBRARY_DIR})
endif(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)

if (PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
    set(PHONON_LIBS ${PHONON_LIBRARY})
    set(PHONON_INCLUDES ${PHONON_INCLUDE_DIR}/KDE ${PHONON_INCLUDE_DIR})
    _phonon_find_version()
endif (PHONON_INCLUDE_DIR AND PHONON_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Phonon DEFAULT_MSG PHONON_INCLUDE_DIR PHONON_LIBRARY)

mark_as_advanced(PHONON_INCLUDE_DIR PHONON_LIBRARY)

