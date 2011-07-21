<version>
    @PHONON_LIB_VERSION@
</version>

<headers>
    @CMAKE_INSTALL_PREFIX@/@INCLUDE_INSTALL_DIR@/phonon
</headers>

<libs>
    @CMAKE_INSTALL_PREFIX@/@LIB_INSTALL_DIR@/libphonon.so.@PHONON_LIB_VERSION@
    @CMAKE_INSTALL_PREFIX@/@LIB_INSTALL_DIR@/libphononexperimental.so.@PHONON_LIB_VERSION@
</libs>
