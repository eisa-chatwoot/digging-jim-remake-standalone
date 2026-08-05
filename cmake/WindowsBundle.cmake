include_guard(GLOBAL)

function(digging_jim_configure_windows_bundle game_target builder_target)
    if(NOT WIN32)
        message(FATAL_ERROR "WindowsBundle.cmake can only run on Windows.")
    endif()

    # The package intentionally contains both executables in one directory.
    # This keeps their shared assets in one place and lets the Builder launch
    # DiggingJim.exe for its Test Level action.
    set(_package_name "DiggingJim-${PROJECT_VERSION}-windows-x64")
    set(_package_directory "${CMAKE_BINARY_DIR}/package")
    set(_package_root "${_package_directory}/${_package_name}")
    set(_package_archive "${CMAKE_BINARY_DIR}/dist/${_package_name}.zip")

    # wxWidgets is supplied as static libraries by the default
    # x64-windows-static-md triplet.  The MSVC runtime remains dynamic, so
    # include it app-locally rather than requiring recipients to install it.
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS TRUE)
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)

    # All application dependencies are linked statically through the
    # x64-windows-static-md triplet.  Do not invoke CMake's transitive DLL
    # scanner here: it follows Windows API-set forwarders and can falsely
    # report unavailable OS-internal DLLs. InstallRequiredSystemLibraries
    # above already copies the required MSVC and UCRT files beside the apps.
    install(TARGETS "${game_target}" "${builder_target}" RUNTIME DESTINATION ".")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "."
        PATTERN "icons_bak" EXCLUDE
        PATTERN ".DS_Store" EXCLUDE
        PATTERN "._*" EXCLUDE)
    install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.md" DESTINATION ".")

    add_custom_target(windows_bundle
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${_package_root}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${_package_directory}"
        COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config Release --prefix "${_package_root}"
        COMMAND "${CMAKE_COMMAND}"
            "-DPACKAGE_ROOT=${_package_root}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/WindowsBundleVerify.cmake"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/dist"
        COMMAND "${CMAKE_COMMAND}" -E rm -f "${_package_archive}"
        COMMAND "${CMAKE_COMMAND}" -E chdir "${_package_directory}"
            "${CMAKE_COMMAND}" -E tar cf "${_package_archive}" --format=zip "${_package_name}"
        DEPENDS "${game_target}" "${builder_target}"
        COMMENT "Creating self-contained Windows x64 distribution package"
        VERBATIM)

    # The ZIP stays available as the transparent portable release.  The two
    # optional single-file EXEs embed that ZIP and unpack it into a versioned
    # per-user cache on first launch, so recipients may also receive exactly
    # one EXE for the game or Builder.
    set(_launcher_source_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/windows-launcher")
    set(_launcher_build_dir "${CMAKE_BINARY_DIR}/single-file-launcher")
    set(_launcher_configure_command
        "${CMAKE_COMMAND}"
        -S "${_launcher_source_dir}"
        -B "${_launcher_build_dir}"
        "-DPAYLOAD_ARCHIVE:FILEPATH=${_package_archive}"
        "-DPACKAGE_NAME:STRING=${_package_name}"
        "-DDIGGING_JIM_PROJECT_VERSION:STRING=${PROJECT_VERSION}"
        "-DOUTPUT_DIRECTORY:PATH=${CMAKE_BINARY_DIR}/dist")
    if(CMAKE_GENERATOR)
        list(APPEND _launcher_configure_command -G "${CMAKE_GENERATOR}")
    endif()
    if(CMAKE_GENERATOR_PLATFORM)
        list(APPEND _launcher_configure_command -A "${CMAKE_GENERATOR_PLATFORM}")
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
        list(APPEND _launcher_configure_command -T "${CMAKE_GENERATOR_TOOLSET}")
    endif()

    add_custom_target(windows_single_file
        COMMAND ${_launcher_configure_command}
        COMMAND "${CMAKE_COMMAND}" --build "${_launcher_build_dir}" --config Release --parallel 4
        COMMAND "${CMAKE_COMMAND}"
            "-DLAUNCHER_DIRECTORY=${CMAKE_BINARY_DIR}/dist"
            "-DPAYLOAD_ARCHIVE=${_package_archive}"
            "-DPACKAGE_NAME=${_package_name}"
            "-DPROJECT_VERSION=${PROJECT_VERSION}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/WindowsSingleFileVerify.cmake"
        DEPENDS windows_bundle
        COMMENT "Embedding the Windows package in single-file launchers"
        VERBATIM)

    add_custom_target(windows_release DEPENDS windows_single_file)

    message(STATUS "Windows standalone package targets enabled: windows_bundle, windows_single_file, windows_release")
endfunction()
