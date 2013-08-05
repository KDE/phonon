set(PHONON_LOGO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/phonon-logo.png)
set(PHONON_DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(PHONON_BUILD_DOC_QCH "yes")
set(PHONON_DOXYGEN_QHP_NAMESPACE "org.kde.phonon.${PHONON_LIB_VERSION}")

set(PHONON_DOXYGEN_SRCS
    ${PHONON_SOURCE_DIR}/Mainpage.dox
    ${PHONON_SOURCE_DIR}/doc/Backend.dox
    ${PHONON_SOURCE_DIR}/doc/Phonon4Qt5.dox
    ${PHONON_SOURCE_DIR}/phonon
)
string(REGEX REPLACE ";" " " PHONON_DOXYGEN_SRCS "${PHONON_DOXYGEN_SRCS}")

set(PHONON_DOXYGEN_EXCLUDE
    ${PHONON_SOURCE_DIR}/phonon/backend
    ${PHONON_SOURCE_DIR}/phonon/experimental
)
string(REGEX REPLACE ";" " " PHONON_DOXYGEN_EXCLUDE "${PHONON_DOXYGEN_EXCLUDE}")


configure_file(Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_target("html" ALL
    doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_target("qch" ALL DEPENDS "html")

add_custom_target("doc" ALL DEPENDS "html" "qch")

INSTALL(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html/" DESTINATION "${SHARE_INSTALL_PREFIX}/libphonon/" )
