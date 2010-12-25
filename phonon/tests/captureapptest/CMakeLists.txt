set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

phonon_add_executable(captureapp capture_test_main.cpp capture_test.cpp)
target_link_libraries(captureapp ${PHONON_LIBS})
set_target_properties(captureapp PROPERTIES COMPILE_FLAGS "-Wall -g")
