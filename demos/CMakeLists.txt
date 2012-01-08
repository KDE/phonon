set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(phota)
add_subdirectory(simpleplayer)
add_subdirectory(metadatareader)

if (NOT PHONON_NO_CAPTURE)
    add_subdirectory(simplecapture)
endif (NOT PHONON_NO_CAPTURE)
