# Defines marcos:
#   TODO doc
#
# Copyright (C) 2010 Harald Sitter <sitter@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(_SET_FANCY _var _value _comment)
    if (NOT DEFINED ${_var})
        set(${_var} "${_value}")
    else (NOT DEFINED ${_var})
        set(${_var} "${${_var}}" CACHE PATH "${_comment}")
    endif (NOT DEFINED ${_var})
endmacro(_SET_FANCY)

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )

if (WIN32)
    _set_fancy(EXEC_INSTALL_PREFIX     "."                                         "Base directory for executables and libraries")
else (WIN32)
    _set_fancy(EXEC_INSTALL_PREFIX     "${CMAKE_INSTALL_PREFIX}"                   "Base directory for executables and libraries")
endif (WIN32)
_set_fancy(SHARE_INSTALL_PREFIX        "${EXEC_INSTALL_PREFIX}/share"              "Base directory for files which go to share/")
_set_fancy(INCLUDE_INSTALL_DIR         "${EXEC_INSTALL_PREFIX}/include"            "The subdirectory to the header prefix")
_set_fancy(BIN_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/bin"                "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
_set_fancy(LIB_INSTALL_DIR             "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}"   "The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})")
_set_fancy(PLUGIN_INSTALL_DIR          "${LIB_INSTALL_DIR}/kde4"                   "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
_set_fancy(ICON_INSTALL_DIR            "${SHARE_INSTALL_PREFIX}/icons"             "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
_set_fancy(SERVICES_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/kde4/services"     "The install dir for service (desktop, protocol, ...) files")
_set_fancy(DBUS_INTERFACES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" "The dbus interfaces install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
_set_fancy(DBUS_SERVICES_INSTALL_DIR   "${SHARE_INSTALL_PREFIX}/dbus-1/services"   "The dbus services install dir (default: ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

set(INSTALL_TARGETS_DEFAULT_ARGS RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                 LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                 ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel)

if(APPLE)
    set(INSTALL_TARGETS_DEFAULT_ARGS ${INSTALL_TARGETS_DEFAULT_ARGS}
                                     BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}")
endif(APPLE)
