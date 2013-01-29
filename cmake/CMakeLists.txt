install(FILES cmake_uninstall.cmake.in
              COPYING-CMAKE-SCRIPTS
              FindAutomoc4.cmake
              FindPackageHandleStandardArgs.cmake
              FindPhononInternal.cmake
              FindQt4.cmake
              MacroEnsureVersion.cmake
              MacroLogFeature.cmake
              MacroOptionalFindPackage.cmake
              MacroPushRequiredVars.cmake
              PhononMacros.cmake
              Qt4ConfigDependentSettings.cmake
              Qt4Macros.cmake
        DESTINATION ${BUILDSYSTEM_INSTALL_DIR})
