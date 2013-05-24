install(FILES cmake_uninstall.cmake.in
              COPYING-CMAKE-SCRIPTS
              FindPackageHandleStandardArgs.cmake
              FindPhononInternal.cmake
              MacroEnsureVersion.cmake
              MacroLogFeature.cmake
              MacroOptionalFindPackage.cmake
              MacroPushRequiredVars.cmake
              PhononMacros.cmake
        DESTINATION ${BUILDSYSTEM_INSTALL_DIR})
