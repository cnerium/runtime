/**
 * @file runtime.hpp
 * @brief cnerium::runtime — Main public header for the Runtime module
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Aggregates the main public headers of the Cnerium Runtime module.
 *
 * Include this file when you want access to the full runtime API:
 *   - version information
 *   - task abstraction
 *   - runtime configuration
 *   - low-level concurrency helpers
 *   - task queue primitives
 *   - worker and thread pool
 *   - executor and scheduler
 *   - high-level runtime facade
 *   - runtime/server integration
 *
 * Notes:
 *   - This is the preferred include for end users of the module
 *   - Internal detail headers are re-exported here for completeness
 *   - Higher-level framework APIs will later be exposed by cnerium/cnerium
 *
 * Usage:
 * @code
 *   #include <cnerium/runtime/runtime.hpp>
 *
 *   using namespace cnerium::runtime;
 *
 *   Runtime runtime;
 *   runtime.start();
 *
 *   runtime.post([]()
 *   {
 *     // work
 *   });
 * @endcode
 */

#pragma once

#include <cnerium/runtime/version.hpp>
#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/RuntimeConfig.hpp>

#include <cnerium/runtime/detail/ConcurrencyUtils.hpp>
#include <cnerium/runtime/detail/StopToken.hpp>
#include <cnerium/runtime/detail/ThreadName.hpp>
#include <cnerium/runtime/detail/BlockingQueue.hpp>
#include <cnerium/runtime/detail/TaskQueue.hpp>

#include <cnerium/runtime/Worker.hpp>
#include <cnerium/runtime/ThreadPool.hpp>
#include <cnerium/runtime/Executor.hpp>
#include <cnerium/runtime/Scheduler.hpp>
#include <cnerium/runtime/Runtime.hpp>
#include <cnerium/runtime/ServerRunner.hpp>
