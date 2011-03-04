set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(phota)
add_subdirectory(simpleplayer)

if (NOT PHONON_NO_CAPTURE)
add_subdirectory(simplecapture)
endif (NOT PHONON_NO_CAPTURE)
