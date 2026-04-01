/**
 * @file Executor.hpp
 * @brief cnerium::runtime — Task submission interface
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the Executor class used by the Cnerium Runtime to provide
 * a clean task submission interface on top of a ThreadPool.
 *
 * An Executor:
 *   - references an existing ThreadPool
 *   - submits tasks into the pool queue
 *   - provides a small high-level API for execution requests
 *
 * Responsibilities:
 *   - expose a simple post(task) API
 *   - validate executor state
 *   - keep task submission separate from pool ownership
 *
 * Design goals:
 *   - Header-only
 *   - Lightweight
 *   - Non-owning
 *   - Easy integration with Runtime and ServerRunner
 *
 * Notes:
 *   - Executor does not own the thread pool
 *   - Executor is intentionally small and explicit
 *   - Task execution order remains controlled by the underlying pool queue
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   ThreadPool pool;
 *   pool.start();
 *
 *   Executor executor(pool);
 *   executor.post([]()
 *   {
 *     // work
 *   });
 * @endcode
 */

#pragma once

#include <utility>

#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/ThreadPool.hpp>

namespace cnerium::runtime
{
  /**
   * @brief Non-owning task submission interface for a ThreadPool.
   */
  class Executor
  {
  public:
    using task_type = cnerium::runtime::Task;
    using pool_type = cnerium::runtime::ThreadPool;

    /// @brief Default constructor.
    Executor() = default;

    /**
     * @brief Construct an executor bound to an existing thread pool.
     *
     * @param pool Target thread pool
     */
    explicit Executor(pool_type &pool) noexcept
        : pool_(&pool)
    {
    }

    Executor(const Executor &) = default;
    Executor &operator=(const Executor &) = default;
    Executor(Executor &&) noexcept = default;
    Executor &operator=(Executor &&) noexcept = default;

    /**
     * @brief Bind the executor to a thread pool.
     *
     * @param pool Target thread pool
     * @return Executor& Self
     */
    Executor &bind(pool_type &pool) noexcept
    {
      pool_ = &pool;
      return *this;
    }

    /**
     * @brief Remove the currently bound thread pool.
     *
     * @return Executor& Self
     */
    Executor &reset() noexcept
    {
      pool_ = nullptr;
      return *this;
    }

    /**
     * @brief Returns true if the executor is bound to a valid thread pool.
     *
     * @return true if valid
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return pool_ != nullptr;
    }

    /**
     * @brief Explicit boolean conversion.
     *
     * @return true if valid
     */
    explicit operator bool() const noexcept
    {
      return valid();
    }

    /**
     * @brief Returns the bound thread pool pointer.
     *
     * @return pool_type* Pool pointer, or nullptr
     */
    [[nodiscard]] pool_type *pool() noexcept
    {
      return pool_;
    }

    /**
     * @brief Returns the bound thread pool pointer.
     *
     * @return const pool_type* Pool pointer, or nullptr
     */
    [[nodiscard]] const pool_type *pool() const noexcept
    {
      return pool_;
    }

    /**
     * @brief Submit a task for asynchronous execution.
     *
     * If no pool is bound, the task is ignored.
     *
     * @param task Task to submit
     * @return bool true if submitted, false otherwise
     */
    [[nodiscard]] bool post(task_type task)
    {
      if (!pool_)
      {
        return false;
      }

      pool_->post(std::move(task));
      return true;
    }

    /**
     * @brief Submit a task for execution.
     *
     * This currently behaves the same as post().
     *
     * @param task Task to submit
     * @return bool true if submitted, false otherwise
     */
    [[nodiscard]] bool dispatch(task_type task)
    {
      return post(std::move(task));
    }

  private:
    pool_type *pool_{nullptr};
  };

} // namespace cnerium::runtime
