prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@/@PHONON_LIB_SONAME@

Name: @PHONON_LIB_SONAME_CAMEL@
Description: Phonon library needed to build applications
Version: @PHONON_VERSION_MAJOR@.@PHONON_VERSION_MINOR@.@PHONON_VERSION_PATCH@
# FIXME: it might be nice to generate these from the target itself...
Requires: Qt5Core, Qt5Gui
Libs: -L${libdir} -l@PHONON_LIB_SONAME@
Cflags: -I${includedir}
