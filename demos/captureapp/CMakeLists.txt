
phonon_add_executable(captureapp captureapp_main.cpp captureapp.cpp)
target_link_libraries(captureapp ${PHONON_LIBS})
set_target_properties(captureapp PROPERTIES COMPILE_FLAGS "-Wall -g")
