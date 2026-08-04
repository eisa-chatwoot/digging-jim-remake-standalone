if(NOT APPLE)
    message(FATAL_ERROR "MacOSBundleFixup.cmake can only run on macOS.")
endif()

foreach(_required_variable GAME_APP BUILDER_APP BUILDER_HELPER ASSETS_DIR)
    if(NOT DEFINED ${_required_variable} OR "${${_required_variable}}" STREQUAL "")
        message(FATAL_ERROR "Missing required variable: ${_required_variable}")
    endif()
endforeach()

if(NOT EXISTS "${GAME_APP}")
    message(FATAL_ERROR "Game bundle does not exist: ${GAME_APP}")
endif()
if(NOT EXISTS "${BUILDER_APP}")
    message(FATAL_ERROR "Builder bundle does not exist: ${BUILDER_APP}")
endif()
if(NOT EXISTS "${BUILDER_HELPER}")
    message(FATAL_ERROR "Builder game helper does not exist: ${BUILDER_HELPER}")
endif()
if(NOT EXISTS "${ASSETS_DIR}")
    message(FATAL_ERROR "Assets directory does not exist: ${ASSETS_DIR}")
endif()

foreach(_bundle IN ITEMS "${GAME_APP}" "${BUILDER_APP}")
    file(COPY "${ASSETS_DIR}" DESTINATION "${_bundle}/Contents/Resources")
endforeach()

if(DEFINED SEARCH_DIRS AND NOT "${SEARCH_DIRS}" STREQUAL "")
    string(REPLACE "|" ";" SEARCH_DIRS "${SEARCH_DIRS}")
endif()

include(BundleUtilities)

fixup_bundle("${GAME_APP}" "" "${SEARCH_DIRS}")
fixup_bundle("${BUILDER_APP}" "${BUILDER_HELPER}" "${SEARCH_DIRS}")

function(verify_bundle bundle)
    file(GLOB_RECURSE _candidates LIST_DIRECTORIES FALSE
        "${bundle}/Contents/MacOS/*"
        "${bundle}/Contents/Frameworks/*"
    )

    foreach(_candidate IN LISTS _candidates)
        execute_process(
            COMMAND otool -L "${_candidate}"
            RESULT_VARIABLE _result
            OUTPUT_VARIABLE _dependencies
            ERROR_QUIET
        )
        if(_result EQUAL 0 AND _dependencies MATCHES "/opt/homebrew/|/usr/local/(opt|Cellar)/")
            message(FATAL_ERROR
                "${_candidate} still refers to a developer-local library:\n${_dependencies}"
            )
        endif()
    endforeach()
endfunction()

verify_bundle("${GAME_APP}")
verify_bundle("${BUILDER_APP}")

if(NOT DEFINED CODESIGN_IDENTITY OR "${CODESIGN_IDENTITY}" STREQUAL "")
    set(CODESIGN_IDENTITY "-")
endif()

foreach(_bundle IN ITEMS "${GAME_APP}" "${BUILDER_APP}")
    execute_process(
        COMMAND codesign --force --deep --sign "${CODESIGN_IDENTITY}" "${_bundle}"
        RESULT_VARIABLE _codesign_result
        OUTPUT_VARIABLE _codesign_output
        ERROR_VARIABLE _codesign_error
    )
    if(NOT _codesign_result EQUAL 0)
        message(FATAL_ERROR "Unable to sign ${_bundle}:\n${_codesign_output}${_codesign_error}")
    endif()

    execute_process(
        COMMAND codesign --verify --deep --strict "${_bundle}"
        RESULT_VARIABLE _verify_result
        OUTPUT_VARIABLE _verify_output
        ERROR_VARIABLE _verify_error
    )
    if(NOT _verify_result EQUAL 0)
        message(FATAL_ERROR "Code signature verification failed for ${_bundle}:\n${_verify_output}${_verify_error}")
    endif()
endforeach()

message(STATUS "macOS bundles are self-contained: ${GAME_APP} and ${BUILDER_APP}")
