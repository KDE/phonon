# Find Phonon

# Copyright (c) 2010, Harald Sitter <sitter@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (NOT PHONON_BUILDSYSTEM_DIR)
    find_program(PC_EXECUTABLE NAMES pkg-config
    PATH_SUFFIXES bin               
    HINTS
    ${CMAKE_INSTALL_PREFIX}
    ONLY_CMAKE_FIND_ROOT_PATH
    )

    if (NOT PC_EXECUTABLE)
        if (Phonon_FIND_REQUIRED)
            message(FATAL_ERROR "ERROR: Could not find pkg-config [required to find Phonon].")
        endif (Phonon_FIND_REQUIRED)
    endif (NOT PC_EXECUTABLE)

    if (PC_EXECUTABLE)
        execute_process(COMMAND "${PC_EXECUTABLE}" --variable=buildsystemdir phonon
                        OUTPUT_VARIABLE _data_DIR
                        ERROR_QUIET
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        file(TO_CMAKE_PATH "${_data_DIR}" _data_DIR)
        find_path(PHONON_BUILDSYSTEM_DIR FindPhononInternal.cmake HINTS ${_data_DIR})
    endif (PC_EXECUTABLE)
endif (NOT PHONON_BUILDSYSTEM_DIR)

if (PHONON_BUILDSYSTEM_DIR)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PHONON_BUILDSYSTEM_DIR})

    if (Phonon_FIND_REQUIRED)
        set(_req REQUIRED)
    endif (Phonon_FIND_REQUIRED)
    if (PHONON_FIND_QUIETLY)
        set(_quiet QUIET)
    endif (PHONON_FIND_QUIETLY)

    find_package(PhononInternal ${_req} ${_quiet})
else (PHONON_BUILDSYSTEM_DIR)
    if (_data_DIR)
        if (Phonon_FIND_REQUIRED)
            message(FATAL_ERROR "ERROR: FindPhonon.cmake not found in ${_data_DIR}")
        endif (Phonon_FIND_REQUIRED)
    else (_data_DIR)
        if (Phonon_FIND_REQUIRED)
            message(FATAL_ERROR "ERROR: Either pkg-config can not find its phonon config, or you are not using a recent enough Phonon version.")
        endif (Phonon_FIND_REQUIRED)
    endif (_data_DIR)
endif (PHONON_BUILDSYSTEM_DIR)
