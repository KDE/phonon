set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(QT_MAJOR_VERSION 5) # only set this in here! we need it in a separate scope
add_subdirectory(phota)
add_subdirectory(simpleplayer)
add_subdirectory(metadatareader)

if (NOT PHONON_NO_CAPTURE)
    add_subdirectory(simplecapture)
endif (NOT PHONON_NO_CAPTURE)
