set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/includes)
add_executable(guitest
   anglewidget.cpp
   audiooutputitem.cpp
   chapterwidget.cpp
   effectitem.cpp
   main.cpp
   mediaobjectitem.cpp
   mygraphicsscene.cpp
   navigationwidget.cpp
   pathitem.cpp
   titlewidget.cpp
   widgetrectitem.cpp
   )
target_link_libraries(guitest ${PHONON_LIBS})
