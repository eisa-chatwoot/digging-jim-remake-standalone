cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    elseif(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
endif()
if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static-md" CACHE STRING "")
endif()
if(WIN32 AND NOT DEFINED CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM "x64" CACHE INTERNAL "")
endif()

project(DiggingJim VERSION 2.0.1 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

if(APPLE)
    set(DIGGING_JIM_CODESIGN_IDENTITY "-" CACHE STRING "Code-signing identity for macOS bundles; use - for ad hoc signing")
    include(cmake/MacOSBundle.cmake)
elseif(WIN32)
    include(cmake/WindowsBundle.cmake)
endif()

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
)
FetchContent_MakeAvailable(ImGui)

set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "imgui top-level directory" FORCE)

FetchContent_Declare(
    ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(ImGui-SFML)

if(WIN32)
    if(DEFINED VCPKG_INSTALLED_DIR)
        set(_vcpkg_share "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share")
    elseif(DEFINED ENV{VCPKG_ROOT})
        set(_vcpkg_share "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share")
    else()
        set(_vcpkg_share "C:/vcpkg/installed/${VCPKG_TARGET_TRIPLET}/share")
    endif()
    if(NOT DEFINED wxWidgets_DIR AND EXISTS "${_vcpkg_share}/wxwidgets")
        set(wxWidgets_DIR "${_vcpkg_share}/wxwidgets")
    endif()
    if(NOT DEFINED NanoSVG_DIR AND EXISTS "${_vcpkg_share}/nanosvg")
        set(NanoSVG_DIR "${_vcpkg_share}/nanosvg")
    endif()
endif()
if(WIN32)
    find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base)
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base)
    if(NOT TARGET wx::wxbase)
        add_library(wx::wxbase INTERFACE IMPORTED GLOBAL)
        target_include_directories(wx::wxbase INTERFACE ${wxWidgets_INCLUDE_DIRS})
        target_compile_definitions(wx::wxbase INTERFACE ${wxWidgets_DEFINITIONS})
        target_link_libraries(wx::wxbase INTERFACE ${wxWidgets_LIBRARIES})
        add_library(wx::wxcore INTERFACE IMPORTED GLOBAL)
        target_link_libraries(wx::wxcore INTERFACE wx::wxbase)
    endif()
endif()

add_subdirectory(lib)
add_subdirectory(game)
add_subdirectory(editor)

if(APPLE)
    digging_jim_add_macos_deployment_target(DiggingJim DiggingJimBuilder)
elseif(WIN32)
    digging_jim_configure_windows_bundle(DiggingJim DiggingJimBuilder)
endif()
