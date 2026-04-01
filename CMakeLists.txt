cmake_minimum_required(VERSION 3.20)

project(cnerium_runtime
    VERSION 0.1.0
    DESCRIPTION "Cnerium Runtime - Concurrent execution runtime for the Cnerium web framework"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ========================
# Options
# ========================
option(CNERIUM_RUNTIME_BUILD_TESTS "Build runtime tests" ON)
option(CNERIUM_RUNTIME_BUILD_EXAMPLES "Build runtime examples" ON)

# ========================
# Global settings
# ========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ========================
# Dependencies
# ========================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
endif()

find_package(cnerium_server CONFIG QUIET)
find_package(server CONFIG QUIET)

set(CNERIUM_RUNTIME_SERVER_TARGET "")

if(TARGET server::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server::server)
elseif(TARGET server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server)
elseif(TARGET cnerium::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium::server)
elseif(TARGET cnerium_server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium_server)
endif()

if(CNERIUM_RUNTIME_SERVER_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium server target not found.\n"
        "Tried: server::server, server, cnerium::server, cnerium_server\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/server\n"
        "  vix install"
    )
endif()

# ========================
# Library (header-only)
# ========================
add_library(runtime INTERFACE)
add_library(runtime::runtime ALIAS runtime)

target_compile_features(runtime INTERFACE cxx_std_20)

target_link_libraries(runtime
    INTERFACE
        ${CNERIUM_RUNTIME_SERVER_TARGET}
)

target_include_directories(runtime INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ========================
# Threads
# ========================
find_package(Threads REQUIRED)
target_link_libraries(runtime
    INTERFACE
        Threads::Threads
)

# ========================
# Compiler options
# ========================
if(MSVC)
    target_compile_options(runtime INTERFACE /W4 /permissive-)
else()
    target_compile_options(runtime INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Target properties
# ========================
set_target_properties(runtime PROPERTIES
    EXPORT_NAME runtime
)

# ========================
# Examples
# ========================
if(CNERIUM_RUNTIME_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
endif()

# ========================
# Tests
# ========================
if(CNERIUM_RUNTIME_BUILD_TESTS)
    include(CTest)
    enable_testing()

    function(cnerium_add_runtime_test target source_file test_name)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${source_file}")
            add_executable(${target} "tests/${source_file}")
            target_link_libraries(${target} PRIVATE runtime::runtime)
            target_compile_features(${target} PRIVATE cxx_std_20)

            if(MSVC)
                target_compile_options(${target} PRIVATE /W4 /permissive-)
            else()
                target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
            endif()

            add_test(NAME ${test_name} COMMAND ${target})
        endif()
    endfunction()

    cnerium_add_runtime_test(runtime_test_basic         test_basic.cpp         runtime.basic)
    cnerium_add_runtime_test(runtime_test_thread_pool   test_thread_pool.cpp   runtime.thread_pool)
    cnerium_add_runtime_test(runtime_test_executor      test_executor.cpp      runtime.executor)
    cnerium_add_runtime_test(runtime_test_scheduler     test_scheduler.cpp     runtime.scheduler)
    cnerium_add_runtime_test(runtime_test_runtime       test_runtime.cpp       runtime.runtime)
    cnerium_add_runtime_test(runtime_test_server_runner test_server_runner.cpp runtime.server_runner)
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "cnerium runtime configured (${PROJECT_VERSION})")
message(STATUS "Server target:      ${CNERIUM_RUNTIME_SERVER_TARGET}")
message(STATUS "Include dir:        ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:        ${CNERIUM_RUNTIME_BUILD_TESTS}")
message(STATUS "Build examples:     ${CNERIUM_RUNTIME_BUILD_EXAMPLES}")
message(STATUS "Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
