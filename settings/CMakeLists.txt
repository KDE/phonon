project(PhononSettings)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Phonon4Qt5 REQUIRED)

add_definitions(-DPHONON_LIB_SONAME=\"${PHONON_LIB_SONAME}\")

message(AUTHOR_WARNING "need to use ecm translation loader!")

set(SRCS
    backendselection.cpp
    backendselection.ui
    devicepreference.cpp
    devicepreference.ui
    main.cpp
    settings.cpp
    settings.qrc
    settings.ui
)

add_executable(phononsettings ${SRCS})
target_link_libraries(phononsettings
    Phonon::phonon4qt5
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

