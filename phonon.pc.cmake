prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@EXEC_INSTALL_PREFIX@
libdir=@ABS_LIB_INSTALL_DIR@
includedir=@ABS_INCLUDE_INSTALL_DIR@
buildsystemdir=@ABS_BUILDSYSTEM_INSTALL_DIR@

Name: @PHONON_LIB_SONAME_CAMEL@
Description: Phonon library needed to build applications
Version: @PHONON_LIB_MAJOR_VERSION@.@PHONON_LIB_MINOR_VERSION@.@PHONON_LIB_PATCH_VERSION@
Requires: @PHONON_PKG_DEPENDENCIES@
Libs: -L${libdir} -l@PHONON_LIB_SONAME@
Cflags: -I${includedir}
