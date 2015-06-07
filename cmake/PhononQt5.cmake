# Copyright (c) 2008, Matthias Kretz <kretz@kde.org>
# Copyright (c) 2010, Mark Kretschmann <kretschmann@kde.org>
# Copyright (c) 2010-2015, Harald Sitter <sitter@kde.org>
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

find_package(Qt5Core)
macro_log_feature(Qt5Core_FOUND "Qt5 Core (qtbase)" "" "" TRUE)

find_package(Qt5Gui)
macro_log_feature(Qt5Gui_FOUND "Qt5 Gui (qtbase)" "" "" TRUE)

find_package(Qt5Widgets)
macro_log_feature(Qt5Widgets_FOUND "Qt5 Widgets (qtbase)" "" "" TRUE)

#---- compat
# Compat variables for plugins.
function(_QT4_QUERY_QMAKE VAR RESULT)
    get_target_property(QT_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} LOCATION)
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} "-query" ${VAR}
                    RESULT_VARIABLE return_code
                    OUTPUT_VARIABLE output)
    if(NOT return_code)
        file(TO_CMAKE_PATH "${output}" output)
        STRING(REGEX REPLACE "(\r?\n)+$" "" output "${output}")
        set(${RESULT} ${output} PARENT_SCOPE)
    endif(NOT return_code)
endfunction(_QT4_QUERY_QMAKE)

_qt4_query_qmake(QT_INSTALL_IMPORTS QT_IMPORTS_DIR)
_qt4_query_qmake(QT_HOST_DATA QT_MKSPECS_DIR)
_qt4_query_qmake(QT_INSTALL_PLUGINS QT_PLUGINS_DIR)

set(QT_MKSPECS_DIR "${QT_MKSPECS_DIR}/mkspecs")

# ---- more compat
set(QT_INCLUDES ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Widgets_INCLUDE_DIRS}
                ${Qt5DBus_INCLUDE_DIRS})

# ECM

find_package(ECM 1.7.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDECMakeSettings)
include(KDECompilerSettings)
