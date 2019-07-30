# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010-2019, Harald Sitter <sitter@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#-------------------------------------------------------------------------------

# Include Additional Magic

message(AUTHOR_WARNING "TODO: port backends from logfeature to featuresummary")
message(AUTHOR_WARNING "TODO: port backends from icon macros")

set(INCLUDE_INSTALL_DIR "${KDE_INSTALL_INCLUDEDIR}/${PHONON_LIB_SONAME}")
message(AUTHOR_WARNING "TODO: this could be moved to the actual plugins dir, needs review for use tho")
set(PLUGIN_INSTALL_DIR "${KDE_INSTALL_LIBDIR}/qt5")
message(AUTHOR_WARNING "TODO: this used plugin_install_dir but is now relying on ECM")
set(BACKEND_INSTALL_DIR "${KDE_INSTALL_QTPLUGINDIR}/${PHONON_LIB_SONAME}_backend")
message(AUTHOR_WARNING "TODO: do we need this with ecm?")
if(WIN32) # Imported from Phonon VLC
    set(BACKEND_INSTALL_DIR "bin/${PHONON_LIB_SONAME}_backend")
endif()

# ---- more compat
set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Widgets_INCLUDE_DIRS}
                ${Qt5DBus_INCLUDE_DIRS})

# NOTE: the compatiibility macros are actively used by the backends, so they
#       cannot be dropped unless the backends get a major release removing all
#       use of them first.

# Compatiblity Macros for old automoc nonesense
macro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
    add_executable(${_target_NAME} ${ARGN})
endmacro(AUTOMOC4_ADD_EXECUTABLE _target_NAME)
macro(AUTOMOC4_ADD_LIBRARY _target_NAME _add_executable_param)
    add_library(${_target_NAME} ${_add_executable_param} ${ARGN})
endmacro(AUTOMOC4_ADD_LIBRARY)

# Compat for older backends
macro(PHONON_UPDATE_ICONCACHE)
    message(DEPRECATION "PHONON_UPDATE_ICONCACHE is deprecated use ECMInstallIcons;"
        " It updates the cache automatically")
endmacro()
macro(PHONON_INSTALL_ICONS _defaultpath)
    message(DEPRECATION "PHONON_INSTALL_ICONS is deprecated use ECMInstallIcons")
    include(ECMInstallIcons)
    ecm_install_icons(${_defaultpath})
endmacro()
