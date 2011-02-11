set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(simpleplayer)

if (NOT PHONON_NO_CAPTURE)
add_subdirectory(captureapp)
add_subdirectory(captureapp2)
endif (NOT PHONON_NO_CAPTURE)
