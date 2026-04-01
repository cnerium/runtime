cmake_minimum_required(VERSION 3.20)

project(cnerium_runtime
    VERSION 0.2.0
    DESCRIPTION "Cnerium Runtime - Concurrent execution runtime for the Cnerium web framework"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ========================
# Options
# ========================
option(CNERIUM_RUNTIME_BUILD_TESTS "Build runtime tests" OFF)
option(CNERIUM_RUNTIME_BUILD_EXAMPLES "Build runtime examples" OFF)

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

if(NOT TARGET cnerium::server
   AND NOT TARGET cnerium_server
   AND NOT TARGET server::server
   AND NOT TARGET server)
    find_package(cnerium_server CONFIG QUIET)
    find_package(server CONFIG QUIET)
endif()

if(NOT TARGET cnerium::http
   AND NOT TARGET cnerium_http
   AND NOT TARGET http::http
   AND NOT TARGET http)
    find_package(cnerium_http CONFIG QUIET)
    find_package(http CONFIG QUIET)
endif()

if(NOT TARGET cnerium::middleware
   AND NOT TARGET cnerium_middleware
   AND NOT TARGET middleware::middleware
   AND NOT TARGET middleware)
    find_package(cnerium_middleware CONFIG QUIET)
    find_package(middleware CONFIG QUIET)
endif()

if(NOT TARGET cnerium::router
   AND NOT TARGET cnerium_router
   AND NOT TARGET router::router
   AND NOT TARGET router)
    find_package(cnerium_router CONFIG QUIET)
    find_package(router CONFIG QUIET)
endif()

if(NOT TARGET cnerium::json
   AND NOT TARGET cnerium_json
   AND NOT TARGET json::json
   AND NOT TARGET json)
    find_package(cnerium_json CONFIG QUIET)
    find_package(json CONFIG QUIET)
endif()

find_package(Threads REQUIRED)

# ========================
# Resolve targets
# ========================
set(CNERIUM_RUNTIME_SERVER_TARGET "")
set(CNERIUM_RUNTIME_HTTP_TARGET "")
set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET "")
set(CNERIUM_RUNTIME_ROUTER_TARGET "")
set(CNERIUM_RUNTIME_JSON_TARGET "")

if(TARGET cnerium::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium::server)
elseif(TARGET cnerium_server)
    set(CNERIUM_RUNTIME_SERVER_TARGET cnerium_server)
elseif(TARGET server::server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server::server)
elseif(TARGET server)
    set(CNERIUM_RUNTIME_SERVER_TARGET server)
endif()

if(TARGET cnerium::http)
    set(CNERIUM_RUNTIME_HTTP_TARGET cnerium::http)
elseif(TARGET cnerium_http)
    set(CNERIUM_RUNTIME_HTTP_TARGET cnerium_http)
elseif(TARGET http::http)
    set(CNERIUM_RUNTIME_HTTP_TARGET http::http)
elseif(TARGET http)
    set(CNERIUM_RUNTIME_HTTP_TARGET http)
endif()

if(TARGET cnerium::middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET cnerium::middleware)
elseif(TARGET cnerium_middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET cnerium_middleware)
elseif(TARGET middleware::middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET middleware::middleware)
elseif(TARGET middleware)
    set(CNERIUM_RUNTIME_MIDDLEWARE_TARGET middleware)
endif()

if(TARGET cnerium::router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET cnerium::router)
elseif(TARGET cnerium_router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET cnerium_router)
elseif(TARGET router::router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET router::router)
elseif(TARGET router)
    set(CNERIUM_RUNTIME_ROUTER_TARGET router)
endif()

if(TARGET cnerium::json)
    set(CNERIUM_RUNTIME_JSON_TARGET cnerium::json)
elseif(TARGET cnerium_json)
    set(CNERIUM_RUNTIME_JSON_TARGET cnerium_json)
elseif(TARGET json::json)
    set(CNERIUM_RUNTIME_JSON_TARGET json::json)
elseif(TARGET json)
    set(CNERIUM_RUNTIME_JSON_TARGET json)
endif()

# ========================
# Hard validation
# ========================
if(CNERIUM_RUNTIME_SERVER_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium server target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/server\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_HTTP_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium http target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/http\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_MIDDLEWARE_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium middleware target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/middleware\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_ROUTER_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium router target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/router\n"
        "  vix install"
    )
endif()

if(CNERIUM_RUNTIME_JSON_TARGET STREQUAL "")
    message(FATAL_ERROR
        "cnerium json target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @cnerium/json\n"
        "  vix install"
    )
endif()

# ========================
# Library (header-only)
# ========================
add_library(cnerium_runtime INTERFACE)
add_library(cnerium::runtime ALIAS cnerium_runtime)

target_compile_features(cnerium_runtime INTERFACE cxx_std_20)

target_include_directories(cnerium_runtime
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cnerium_runtime
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
    target_compile_options(cnerium_runtime INTERFACE /W4 /permissive-)
else()
    target_compile_options(cnerium_runtime INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Target properties
# ========================
set_target_properties(cnerium_runtime PROPERTIES
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
            target_link_libraries(${target} PRIVATE cnerium::runtime)
            target_compile_features(${target} PRIVATE cxx_std_20)

            if(MSVC)
                target_compile_options(${target} PRIVATE /W4 /permissive-)
            else()
                target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
            endif()

            add_test(NAME ${test_name} COMMAND ${target})
        endif()
    endfunction()

    cnerium_add_runtime_test(cnerium_runtime_test_basic         test_basic.cpp         cnerium_runtime.basic)
    cnerium_add_runtime_test(cnerium_runtime_test_thread_pool   test_thread_pool.cpp   cnerium_runtime.thread_pool)
    cnerium_add_runtime_test(cnerium_runtime_test_executor      test_executor.cpp      cnerium_runtime.executor)
    cnerium_add_runtime_test(cnerium_runtime_test_scheduler     test_scheduler.cpp     cnerium_runtime.scheduler)
    cnerium_add_runtime_test(cnerium_runtime_test_runtime       test_runtime.cpp       cnerium_runtime.runtime)
    cnerium_add_runtime_test(cnerium_runtime_test_server_runner test_server_runner.cpp cnerium_runtime.server_runner)
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "cnerium::runtime configured (${PROJECT_VERSION})")
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
