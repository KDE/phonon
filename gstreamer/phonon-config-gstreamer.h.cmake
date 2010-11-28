/* GStreamer definitions from CMake */

/* VERSION with each part shifted by 8 from the lower part */
#define GST_VERSION ((GST_VERSION_MAJOR<<24)|(GST_VERSION_MINOR<<16)|(GST_VERSION_MICRO<<8)|(GST_VERSION_NANO))

/* Can be used like GST_VERSION <= GST_VERSION_CHECK(0,10,30,0) */
#define GST_VERSION_CHECK(major, minor, micro, nano) ((major<<24)|(minor<<16)|(micro<<8)|(nano))

/* If api-plugin is defined */
#cmakedefine PLUGIN_INSTALL_API 1

