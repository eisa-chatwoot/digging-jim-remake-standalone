cmake_minimum_required(VERSION 3.28)

project(DiggingJimSingleFileLaunchers LANGUAGES CXX RC)

if(NOT WIN32)
    message(FATAL_ERROR "The Windows single-file launchers can only be built on Windows.")
endif()

foreach(_required_variable IN ITEMS
        PAYLOAD_ARCHIVE
        PACKAGE_NAME
        DIGGING_JIM_PROJECT_VERSION
        OUTPUT_DIRECTORY)
    if(NOT DEFINED ${_required_variable} OR "${${_required_variable}}" STREQUAL "")
        message(FATAL_ERROR "${_required_variable} is required.")
    endif()
endforeach()

if(NOT EXISTS "${PAYLOAD_ARCHIVE}")
    message(FATAL_ERROR "The Windows package payload does not exist: ${PAYLOAD_ARCHIVE}")
endif()

file(SHA256 "${PAYLOAD_ARCHIVE}" PAYLOAD_SHA256)
file(TO_CMAKE_PATH "${PAYLOAD_ARCHIVE}" PAYLOAD_ARCHIVE_RC_PATH)

# The outer launchers must work before the embedded portable package is
# extracted, so link the MSVC runtime statically into the launcher itself.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

function(digging_jim_add_single_file_launcher target output_name launch_executable icon_path)
    if(NOT EXISTS "${icon_path}")
        message(FATAL_ERROR "Launcher icon does not exist: ${icon_path}")
    endif()

    file(TO_CMAKE_PATH "${icon_path}" APPLICATION_ICON_RC_PATH)
    set(LAUNCHER_PRODUCT_NAME "${output_name}")
    set(LAUNCHER_EXECUTABLE "${launch_executable}")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/SingleFileLauncher.cpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.cpp"
        @ONLY)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/SingleFileLauncher.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc"
        @ONLY)

    add_executable(${target} WIN32
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc")
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
    target_link_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/SUBSYSTEM:WINDOWS>)
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${output_name}"
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
endfunction()

digging_jim_add_single_file_launcher(
    DiggingJimSingleFile
    "DiggingJim-${DIGGING_JIM_PROJECT_VERSION}-windows-x64"
    "DiggingJim.exe"
    "${CMAKE_CURRENT_LIST_DIR}/../../assets/icons/DiggingJim/large.ico")

digging_jim_add_single_file_launcher(
    DiggingJimBuilderSingleFile
    "DiggingJimBuilder-${DIGGING_JIM_PROJECT_VERSION}-windows-x64"
    "DiggingJimBuilder.exe"
    "${CMAKE_CURRENT_LIST_DIR}/../../assets/icons/DiggingJimBuilder/large.ico")
