if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

if(NOT WIN32)
    message(FATAL_ERROR "WindowsSingleFileVerify.cmake can only run on Windows.")
endif()

foreach(_required_variable IN ITEMS LAUNCHER_DIRECTORY PAYLOAD_ARCHIVE PACKAGE_NAME PROJECT_VERSION)
    if(NOT DEFINED ${_required_variable} OR "${${_required_variable}}" STREQUAL "")
        message(FATAL_ERROR "${_required_variable} is required.")
    endif()
endforeach()

if(NOT EXISTS "${PAYLOAD_ARCHIVE}")
    message(FATAL_ERROR "The embedded ZIP payload does not exist: ${PAYLOAD_ARCHIVE}")
endif()

file(SIZE "${PAYLOAD_ARCHIVE}" _payload_size)
set(_launchers
    "${LAUNCHER_DIRECTORY}/DiggingJim-${PROJECT_VERSION}-windows-x64.exe"
    "${LAUNCHER_DIRECTORY}/DiggingJimBuilder-${PROJECT_VERSION}-windows-x64.exe")

foreach(_launcher IN LISTS _launchers)
    if(NOT EXISTS "${_launcher}")
        message(FATAL_ERROR "Single-file launcher was not created: ${_launcher}")
    endif()
    file(SIZE "${_launcher}" _launcher_size)
    if(_launcher_size LESS _payload_size)
        message(FATAL_ERROR
            "Single-file launcher does not contain the full ZIP payload: ${_launcher}")
    endif()
endforeach()

# A launcher's own runtime must be static. Its only external dependencies may
# be Windows system DLLs; the embedded ZIP supplies every non-system runtime
# required by the game and Builder after extraction.
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES ${_launchers}
    RESOLVED_DEPENDENCIES_VAR _resolved_dependencies
    UNRESOLVED_DEPENDENCIES_VAR _unresolved_dependencies
    PRE_EXCLUDE_REGEXES
        "api-ms-.*"
        "ext-ms-.*"
    POST_EXCLUDE_REGEXES
        ".*[\\\\/]system32[\\\\/].*"
        ".*[\\\\/]syswow64[\\\\/].*")

if(_resolved_dependencies)
    message(FATAL_ERROR
        "Single-file launcher has a non-system dependency: ${_resolved_dependencies}")
endif()

set(_unexpected_unresolved_dependencies)
foreach(_dependency IN LISTS _unresolved_dependencies)
    string(TOLOWER "${_dependency}" _dependency_lower)
    if(_dependency_lower MATCHES "^(api-ms-|ext-ms-)")
        continue()
    endif()
    list(APPEND _unexpected_unresolved_dependencies "${_dependency}")
endforeach()
if(_unexpected_unresolved_dependencies)
    message(FATAL_ERROR
        "Single-file launcher has unresolved dependencies: ${_unexpected_unresolved_dependencies}")
endif()

message(STATUS "Windows single-file launchers verified: ${LAUNCHER_DIRECTORY}")
