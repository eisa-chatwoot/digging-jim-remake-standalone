add_executable(DiggingJimBuilder
    src/main.cpp
    src/Editor/Editor.cpp
    src/HUD/Panel.cpp
    src/HUD/Toolbar.cpp
    ${CMAKE_SOURCE_DIR}/game/src/Game/Game.cpp
    ${CMAKE_SOURCE_DIR}/game/src/Game/Settings.cpp
    ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.c
    $<$<BOOL:${WIN32}>:res/builder.rc>
)

target_compile_features(DiggingJimBuilder PRIVATE cxx_std_17)

target_include_directories(DiggingJimBuilder PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/game/include
    ${CMAKE_SOURCE_DIR}/lib/include
    ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs
)

target_link_libraries(DiggingJimBuilder PRIVATE DiggingJimLib)

# The Builder's Test action starts DiggingJim from the same runtime directory.
# Keeping this dependency on every platform also serializes the shared assets
# staging step used by non-bundle builds.
add_dependencies(DiggingJimBuilder DiggingJim)

if(APPLE)
    digging_jim_configure_macos_bundle(
        DiggingJimBuilder
        "Digging Jim Builder"
        "com.retrospade.diggingjim.builder"
        "DiggingJimBuilder"
    )
    add_custom_command(
        TARGET DiggingJimBuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:DiggingJim>
                "$<TARGET_BUNDLE_CONTENT_DIR:DiggingJimBuilder>/MacOS/DiggingJim"
        COMMENT "Embedding the game test helper in DiggingJimBuilder.app"
        VERBATIM
    )
endif()
if(WIN32)
    target_link_libraries(DiggingJimBuilder PRIVATE dwmapi)
    target_link_options(DiggingJimBuilder PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
        $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-mwindows>
    )
endif()
