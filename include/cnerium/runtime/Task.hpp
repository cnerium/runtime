/**
 * @file Task.hpp
 * @brief cnerium::runtime — Executable task abstraction
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the fundamental executable unit used by the Cnerium Runtime.
 *
 * A Task is a lightweight callable object representing one unit of work
 * that can be queued, scheduled, and executed by the runtime.
 *
 * Responsibilities:
 *   - represent one executable work item
 *   - provide a uniform type for queues and executors
 *   - keep the runtime API simple and expressive
 *
 * Design goals:
 *   - Header-only
 *   - Minimal public surface
 *   - Easy lambda integration
 *   - Suitable for thread pool execution
 *
 * Notes:
 *   - A Task is currently defined as std::function<void()>
 *   - This makes it easy to submit lambdas, bind expressions, and callables
 *   - Future optimizations may introduce move-only task wrappers internally
 *     without changing the public runtime architecture
 *
 * Usage:
 * @code
 *   #include <cnerium/runtime/Task.hpp>
 *
 *   cnerium::runtime::Task task = []()
 *   {
 *     // work
 *   };
 *
 *   if (task)
 *   {
 *     task();
 *   }
 * @endcode
 */

#pragma once

#include <functional>

namespace cnerium::runtime
{
  /**
   * @brief Executable unit of work handled by the runtime.
   *
   * A Task stores any callable compatible with the signature:
   *
   *   void()
   *
   * Typical examples:
   *   - lambdas
   *   - free functions
   *   - functors
   *   - std::bind results
   */
  using Task = std::function<void()>;

} // namespace cnerium::runtime
