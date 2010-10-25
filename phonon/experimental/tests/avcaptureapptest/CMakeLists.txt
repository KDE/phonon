
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Phonon's build dir is neccesary for experimental/avcapture.h, 'cause it includes phononnamespace.h,
# which is generated from phononnamespace.h.in
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../..")
phonon_add_executable(avcaptureapp capture_test_main.cpp capture_test.cpp)
target_link_libraries(avcaptureapp ${PHONON_LIBS} phononexperimental)
set_target_properties(avcaptureapp PROPERTIES COMPILE_FLAGS "-Wall -g")
