# Find SystemTap components
# It defines the following variables upon sucess:
#   SYSTEMTAP_DTRACE_EXECUTABLE - The path to the dtrace executable
#   SYSTEMTAP_FOUND - Set to true if systemtap support is available
#
# The following macros are defined:
#   SYSTEMTAP_ADD_TAPSET(target probes.d) - Builds a probes.d.h and probes.d.o from the probes.d file and links the project against the .o
#
# Copyright (c) 2011, Trever Fischer <tdfischer@fedoraproject.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_program(SYSTEMTAP_DTRACE_EXECUTABLE dtrace)
if (SYSTEMTAP_DTRACE_EXECUTABLE)
    set(SYSTEMTAP_FOUND TRUE)
endif(SYSTEMTAP_DTRACE_EXECUTABLE)

if (SYSTEMTAP_FOUND)
    set(SYSTEMTAP_PROBE_INSTALL_DIR /usr/share/systemtap/tapset/ CACHE PATH "Location to install systemtap probe.d files")
endif(SYSTEMTAP_FOUND)

macro(SYSTEMTAP_ADD_TAPSET TARGET PROBES_D)
    file(RELATIVE_PATH _output "${CMAKE_CURRENT_SOURCE_DIR}" "${PROBES_D}")
    set(_o_output "${CMAKE_CURRENT_BINARY_DIR}/${_output}.o")
    set(_h_output "${CMAKE_CURRENT_BINARY_DIR}/${_output}.h")
    add_custom_command(OUTPUT "${_h_output}"
        COMMAND "${SYSTEMTAP_DTRACE_EXECUTABLE}" ARGS -C -h -s ${PROBES_D} -o "${_h_output}"
        DEPENDS ${PROBES_D}
        COMMENT "Building DTrace probe header"
    )
    add_custom_command(OUTPUT "${_o_output}"
        COMMAND "${SYSTEMTAP_DTRACE_EXECUTABLE}" ARGS -G -s ${PROBES_D} -o "${_o_output}"
        DEPENDS ${PROBES_D}
        COMMENT "Building DTrace probe object"
    )
    add_custom_target("${TARGET}_dtrace"
        DEPENDS "${_h_output}" "${_o_output}"
        COMMENT "Building DTrace probes"
    )
    add_dependencies(${TARGET} "${TARGET}_dtrace")
    target_link_libraries(${TARGET} "${_o_output}")
endmacro(SYSTEMTAP_ADD_TAPSET TARGET PROBES_D)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SystemTap DEFAULT_MSG SYSTEMTAP_DTRACE_EXECUTABLE)
