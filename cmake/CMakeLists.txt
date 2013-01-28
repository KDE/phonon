install(FILES cmake_uninstall.cmake.in
              COPYING-CMAKE-SCRIPTS
              FindAutomoc4.cmake
              FindPackageHandleStandardArgs.cmake
              FindPhononInternal.cmake
              PhononMacros.cmake
              MacroEnsureVersion.cmake
              MacroLogFeature.cmake
              MacroOptionalFindPackage.cmake
              MacroPushRequiredVars.cmake
              PhononMacros.cmake
        DESTINATION ${BUILDSYSTEM_INSTALL_DIR})
