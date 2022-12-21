# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

add_library(phonon-quick SHARED plugin.cpp videoitem.cpp)
target_link_libraries(phonon-quick
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Qml
        Qt${QT_MAJOR_VERSION}::Quick
        ${PHONON_LIB_SONAME}
        ${PHONON_LIB_SONAME}experimental
)

set(PRIVATE_QML_INSTALL_DIR ${QML_INSTALL_DIR}/org/kde/phonon)
install(TARGETS phonon-quick DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES qmldir DESTINATION ${PRIVATE_QML_INSTALL_DIR})
