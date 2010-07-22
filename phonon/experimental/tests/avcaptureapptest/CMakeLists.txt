
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

phonon_add_executable(avcaptureapp capture_test_main.cpp capture_test.cpp)
target_link_libraries(avcaptureapp ${PHONON_LIBS} phononexperimental ${PHONON_KDECORE_LIBS})
set_target_properties(avcaptureapp PROPERTIES COMPILE_FLAGS "-Wall -g")
