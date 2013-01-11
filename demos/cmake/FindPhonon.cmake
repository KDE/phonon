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

if(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
    set(PHONON_FIND_QUIETLY TRUE)
    
#     include(FindPackageHandleStandardArgs)
#     find_package_handle_standard_args(Phonon DEFAULT_MSG PHONON_INCLUDE_DIR PHONON_LIBRARY)

#     mark_as_advanced(PHONON_INCLUDE_DIR PHONON_LIBRARY)

else()
    find_package(Phonon NO_MODULE)
    if(PHONON_FOUND)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PHONON_BUILDSYSTEM_DIR})
    endif()

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(Phonon  DEFAULT_MSG  Phonon_DIR )
endif(PHONON_INCLUDE_DIR AND PHONON_LIBRARY)
