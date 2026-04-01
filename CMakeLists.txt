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
# Vix deps
# ========================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
endif()

# ========================
# Dependencies
# ========================
find_package(cnerium_server CONFIG QUIET)
find_package(server CONFIG QUIET)

find_package(cnerium_http CONFIG QUIET)
find_package(http CONFIG QUIET)

find_package(cnerium_middleware CONFIG QUIET)
find_package(middleware CONFIG QUIET)

find_package(cnerium_router CONFIG QUIET)
find_package(router CONFIG QUIET)

find_package(cnerium_json CONFIG QUIET)
find_package(json CONFIG QUIET)

find_package(Threads REQUIRED)

set(CNERIUM_RUNTIME_SERVER_TARGET "")
set(CNERIUM_RUNTIME_HTTP_TARGET "")
set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET "")
set(CNERIUM_RUNTIME_ROUTER_TARGET "")
set(CNERIUM_RUNTIME_JSON_TARGET "")

if(TARGET server::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server::server)
elseif(TARGET server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server)
elseif(TARGET cnerium::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium::server)
elseif(TARGET cnerium_server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium_server)
endif()

if(TARGET http::http)
    set(CNERIUM_RUNTIME_HTTP_TARGET http::http)
elseif(TARGET http)
    set(CNERIUM_RUNTIME_HTTP_TARGET http)
elseif(TARGET cnerium::http)
    set(CNERIUM_RUNTIME_HTTP_TARGET cnerium::http)
elseif(TARGET cnerium_http)
    set(CNERIUM_RUNTIME_HTTP_TARGET cnerium_http)
endif()

if(TARGET middleware::middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET middleware::middleware)
elseif(TARGET middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET middleware)
elseif(TARGET cnerium::middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET cnerium::middleware)
elseif(TARGET cnerium_middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET cnerium_middleware)
endif()

if(TARGET router::router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET router::router)
elseif(TARGET router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET router)
elseif(TARGET cnerium::router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET cnerium::router)
elseif(TARGET cnerium_router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET cnerium_router)
endif()

if(TARGET json::json)
    set(CNERIUM_RUNTIME_JSON_TARGET json::json)
elseif(TARGET json)
    set(CNERIUM_RUNTIME_JSON_TARGET json)
elseif(TARGET cnerium::json)
    set(CNERIUM_RUNTIME_JSON_TARGET cnerium::json)
elseif(TARGET cnerium_json)
    set(CNERIUM_RUNTIME_JSON_TARGET cnerium_json)
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

if(CNERIUM_RUNTIME_HTTP_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium http target not found.\n"
        "Tried: http::http, http, cnerium::http, cnerium_http\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/http\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_MIDDLEWARE_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium middleware target not found.\n"
        "Tried: middleware::middleware, middleware, cnerium::middleware, cnerium_middleware\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/middleware\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_ROUTER_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium router target not found.\n"
        "Tried: router::router, router, cnerium::router, cnerium_router\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/router\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_JSON_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium json target not found.\n"
        "Tried: json::json, json, cnerium::json, cnerium_json\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/json\n"
        "  vix install"
    )
endif()

# ========================
# Library (header-only)
# ========================
add_library(runtime INTERFACE)
add_library(runtime::runtime ALIAS runtime)

target_compile_features(runtime INTERFACE cxx_std_20)

target_include_directories(runtime INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(runtime
    INTERFACE
        ${CNERIUM_RUNTIME_SERVER_TARGET}
        ${CNERIUM_RUNTIME_HTTP_TARGET}
        ${CNERIUM_RUNTIME_MIDDLEWARE_TARGET}
        ${CNERIUM_RUNTIME_ROUTER_TARGET}
        ${CNERIUM_RUNTIME_JSON_TARGET}
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
message(STATUS "HTTP target:        ${CNERIUM_RUNTIME_HTTP_TARGET}")
message(STATUS "Middleware target:  ${CNERIUM_RUNTIME_MIDDLEWARE_TARGET}")
message(STATUS "Router target:      ${CNERIUM_RUNTIME_ROUTER_TARGET}")
message(STATUS "JSON target:        ${CNERIUM_RUNTIME_JSON_TARGET}")
message(STATUS "Include dir:        ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:        ${CNERIUM_RUNTIME_BUILD_TESTS}")
message(STATUS "Build examples:     ${CNERIUM_RUNTIME_BUILD_EXAMPLES}")
message(STATUS "Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
