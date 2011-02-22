project(captureapp2)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Phonon REQUIRED)

add_definitions(${QT_DEFINITIONS} ${PHONON_DEFINITIONS})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PHONON_INCLUDES})

set(captureapp2_SRCS
    captureapp2_main.cpp
    captureapp2.cpp
)

qt4_automoc(${captureapp2_SRCS})

add_executable(captureapp2 ${captureapp2_SRCS} ${captureapp2_MOC_SRCS})

target_link_libraries(captureapp2 ${QT_LIBRARIES} phonon phononexperimental)
