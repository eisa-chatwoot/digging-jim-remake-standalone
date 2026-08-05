include_guard(GLOBAL)

function(digging_jim_configure_macos_bundle target display_name bundle_identifier icon_directory)
    if(NOT APPLE)
        return()
    endif()

    set(_default_icon_source "${CMAKE_SOURCE_DIR}/assets/icons/${icon_directory}/large_png.png")
    set(_app_icon_source "${CMAKE_SOURCE_DIR}/assets/icons/${icon_directory}/app_icon.png")

    if(EXISTS "${_app_icon_source}")
        set(_icon_source "${_app_icon_source}")
    else()
        set(_icon_source "${_default_icon_source}")
    endif()
    set(_icon_name "${target}.icns")
    set(_icon_png "${CMAKE_CURRENT_BINARY_DIR}/${target}-icon-512.png")
    set(_icon_file "${CMAKE_CURRENT_BINARY_DIR}/${_icon_name}")
    set(_license_source "${CMAKE_SOURCE_DIR}/LICENSE.md")

    add_custom_command(
        OUTPUT "${_icon_file}"
        COMMAND /usr/bin/sips -z 512 512 "${_icon_source}" --out "${_icon_png}"
        COMMAND /usr/bin/sips -s format icns "${_icon_png}" --out "${_icon_file}"
        DEPENDS "${_icon_source}"
        COMMENT "Creating macOS icon for ${target}"
        VERBATIM
    )

    set(_icon_target "${target}_macos_icon")
    add_custom_target("${_icon_target}" DEPENDS "${_icon_file}")
    add_dependencies("${target}" "${_icon_target}")

    set_target_properties("${target}" PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "${bundle_identifier}"
        MACOSX_BUNDLE_BUNDLE_NAME "${display_name}"
        MACOSX_BUNDLE_INFO_STRING "${display_name}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${display_name} ${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2026 FlatWhite"
        MACOSX_BUNDLE_ICON_FILE "${_icon_name}"
    )

    add_custom_target("${target}_macos_bundle_resources"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources"
        # copy_directory does not remove files deleted from the source tree.
        # Clear the staged assets first so renamed icon files cannot linger in
        # a rebuilt application bundle.
        COMMAND "${CMAKE_COMMAND}" -E rm -r -f "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/assets"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets"
                "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/assets"
        # Keep original icon backups in the source tree without shipping them.
        COMMAND "${CMAKE_COMMAND}" -E rm -r -f "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/assets/icons_bak"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_icon_file}"
                "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/${_icon_name}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_license_source}"
                "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/LICENSE.md"
        DEPENDS "${target}" "${_icon_file}" "${_license_source}"
        COMMENT "Copying resources into $<TARGET_BUNDLE_DIR:${target}>"
        VERBATIM
    )
endfunction()

function(digging_jim_add_macos_deployment_target game_target builder_target)
    if(NOT APPLE)
        return()
    endif()

    set(_search_directories)
    foreach(_library IN LISTS wxWidgets_LIBRARIES)
        if(IS_ABSOLUTE "${_library}")
            get_filename_component(_directory "${_library}" DIRECTORY)
            list(APPEND _search_directories "${_directory}")
        elseif(_library MATCHES "^-L(.+)$")
            list(APPEND _search_directories "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    foreach(_directory IN LISTS wxWidgets_LIBRARY_DIRS)
        if(IS_ABSOLUTE "${_directory}")
            list(APPEND _search_directories "${_directory}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES _search_directories)
    string(REPLACE ";" "|" _search_directories_argument "${_search_directories}")

    add_custom_target(macos_bundle
        DEPENDS
            "${game_target}_macos_bundle_resources"
            "${builder_target}_macos_bundle_resources"
        COMMAND "${CMAKE_COMMAND}"
            "-DGAME_APP=$<TARGET_BUNDLE_DIR:${game_target}>"
            "-DBUILDER_APP=$<TARGET_BUNDLE_DIR:${builder_target}>"
            "-DBUILDER_HELPER=$<TARGET_BUNDLE_CONTENT_DIR:${builder_target}>/MacOS/DiggingJim"
            "-DASSETS_DIR=${CMAKE_SOURCE_DIR}/assets"
            "-DSEARCH_DIRS=${_search_directories_argument}"
            "-DCODESIGN_IDENTITY=${DIGGING_JIM_CODESIGN_IDENTITY}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/MacOSBundleFixup.cmake"
        COMMENT "Bundling macOS runtime dependencies"
        USES_TERMINAL
        VERBATIM
    )
endfunction()
