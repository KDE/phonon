
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

phonon_add_executable(testapp1 main.cpp phonon_test.cpp)
target_link_libraries(testapp1 ${PHONON_LIBS} kdecore)
set_target_properties(testapp1 PROPERTIES COMPILE_FLAGS "-Wall -g")
