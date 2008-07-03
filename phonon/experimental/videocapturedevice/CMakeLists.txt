project(videocapturetestapp)

include(CheckIncludeFile)
include(CheckIncludeFiles)


check_include_files(linux/videodev2.h V4L2_FOUND)

if(NOT V4L2_FOUND)
  message(FATAL_ERROR "Video4Linux2 not found")
endif(NOT V4L2_FOUND)

find_package(KDE4 REQUIRED)

add_subdirectory( videocapture )
add_subdirectory( videocapturetestapp )
