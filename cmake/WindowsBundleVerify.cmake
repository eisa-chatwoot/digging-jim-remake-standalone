if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

if(NOT WIN32)
    message(FATAL_ERROR "WindowsBundleVerify.cmake can only run on Windows.")
endif()

if(NOT DEFINED PACKAGE_ROOT OR PACKAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "PACKAGE_ROOT is required.")
endif()

file(TO_CMAKE_PATH "${PACKAGE_ROOT}" _package_root)
string(TOLOWER "${_package_root}" _package_root_lower)

foreach(_required IN ITEMS
        "${PACKAGE_ROOT}/DiggingJim.exe"
        "${PACKAGE_ROOT}/DiggingJimBuilder.exe"
        "${PACKAGE_ROOT}/assets"
        "${PACKAGE_ROOT}/LICENSE.md")
    if(NOT EXISTS "${_required}")
        message(FATAL_ERROR "Windows package is incomplete: ${_required}")
    endif()
endforeach()

file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES
        "${PACKAGE_ROOT}/DiggingJim.exe"
        "${PACKAGE_ROOT}/DiggingJimBuilder.exe"
    DIRECTORIES
        "${PACKAGE_ROOT}"
    RESOLVED_DEPENDENCIES_VAR _resolved_dependencies
    UNRESOLVED_DEPENDENCIES_VAR _unresolved_dependencies
    PRE_EXCLUDE_REGEXES
        "api-ms-.*"
        "ext-ms-.*"
    POST_EXCLUDE_REGEXES
        ".*[\\\\/]system32[\\\\/].*"
        ".*[\\\\/]syswow64[\\\\/].*")

foreach(_runtime_pattern IN ITEMS
        "msvcp140*.dll"
        "vcruntime140*.dll"
        "ucrtbase.dll")
    file(GLOB _runtime_files "${PACKAGE_ROOT}/${_runtime_pattern}")
    if(NOT _runtime_files)
        message(FATAL_ERROR
            "Windows package is missing its bundled MSVC/UCRT runtime: ${_runtime_pattern}")
    endif()
endforeach()

foreach(_dependency IN LISTS _resolved_dependencies)
    file(TO_CMAKE_PATH "${_dependency}" _dependency_path)
    string(TOLOWER "${_dependency_path}" _dependency_lower)
    string(FIND "${_dependency_lower}" "${_package_root_lower}/" _inside_package)
    string(FIND "${_dependency_lower}" "c:/windows/" _inside_windows)
    if(NOT _inside_package EQUAL 0 AND NOT _inside_windows EQUAL 0)
        message(FATAL_ERROR
            "Windows package still depends on an external runtime: ${_dependency_path}")
    endif()
endforeach()

set(_known_windows_unresolved
    "azureattestmanager.dll"
    "azureattestnormal.dll"
    "hvsifiletrust.dll"
    "pdmutilities.dll"
    "wpaxholder.dll")
set(_unexpected_unresolved_dependencies)
foreach(_dependency IN LISTS _unresolved_dependencies)
    string(TOLOWER "${_dependency}" _dependency_lower)
    if(_dependency_lower MATCHES "^(api-ms-|ext-ms-)")
        continue()
    endif()
    list(FIND _known_windows_unresolved "${_dependency_lower}" _known_windows_dependency)
    if(_known_windows_dependency EQUAL -1)
        list(APPEND _unexpected_unresolved_dependencies "${_dependency}")
    endif()
endforeach()

if(_unexpected_unresolved_dependencies)
    message(FATAL_ERROR
        "Windows package has unresolved runtime dependencies: ${_unexpected_unresolved_dependencies}")
endif()

message(STATUS "Windows package verified as self-contained: ${PACKAGE_ROOT}")
