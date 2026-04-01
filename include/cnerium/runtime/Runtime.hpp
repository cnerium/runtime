/**
 * @file Runtime.hpp
 * @brief cnerium::runtime — High-level runtime facade
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the Runtime class, the main high-level facade of the
 * Cnerium Runtime module.
 *
 * A Runtime owns and coordinates:
 *   - the runtime configuration
 *   - the thread pool
 *   - the executor
 *   - the scheduler
 *
 * Responsibilities:
 *   - initialize the execution engine
 *   - expose a simple lifecycle API
 *   - provide access to pool, executor, and scheduler
 *   - centralize runtime ownership in one object
 *
 * Design goals:
 *   - Header-only
 *   - Clear ownership model
 *   - Safe default behavior
 *   - Easy integration with ServerRunner and future App layer
 *
 * Notes:
 *   - Runtime owns the ThreadPool
 *   - Executor and Scheduler are non-owning views bound to owned objects
 *   - start() launches the pool workers
 *   - stop() requests cooperative shutdown
 *   - join() waits for all worker threads to finish
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   Runtime runtime;
 *   runtime.start();
 *
 *   runtime.post([]()
 *   {
 *     // work
 *   });
 *
 *   runtime.stop();
 *   runtime.join();
 * @endcode
 */

#pragma once

#include <utility>

#include <cnerium/runtime/Executor.hpp>
#include <cnerium/runtime/RuntimeConfig.hpp>
#include <cnerium/runtime/Scheduler.hpp>
#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/ThreadPool.hpp>

namespace cnerium::runtime
{
  /**
   * @brief High-level runtime facade owning the execution engine.
   */
  class Runtime
  {
  public:
    using config_type = cnerium::runtime::RuntimeConfig;
    using pool_type = cnerium::runtime::ThreadPool;
    using executor_type = cnerium::runtime::Executor;
    using scheduler_type = cnerium::runtime::Scheduler;
    using task_type = cnerium::runtime::Task;

    /**
     * @brief Construct a runtime with default configuration.
     */
    Runtime()
        : Runtime(config_type{})
    {
    }

    /**
     * @brief Construct a runtime with explicit configuration.
     *
     * @param config Runtime configuration
     */
    explicit Runtime(config_type config)
        : config_(std::move(config)),
          pool_(config_),
          executor_(pool_),
          scheduler_(executor_)
    {
    }

    Runtime(const Runtime &) = delete;
    Runtime &operator=(const Runtime &) = delete;
    Runtime(Runtime &&) = delete;
    Runtime &operator=(Runtime &&) = delete;

    /**
     * @brief Destructor.
     *
     * Requests stop and joins all workers if needed.
     */
    ~Runtime()
    {
      stop();
      join();
    }

    /**
     * @brief Start the runtime.
     *
     * Starts the underlying thread pool.
     *
     * @return Runtime& Self
     */
    Runtime &start()
    {
      pool_.start();
      return *this;
    }

    /**
     * @brief Request cooperative stop of the runtime.
     *
     * Stops the underlying thread pool.
     *
     * @return Runtime& Self
     */
    Runtime &stop() noexcept
    {
      pool_.stop();
      return *this;
    }

    /**
     * @brief Wait for all worker threads to finish.
     *
     * @return Runtime& Self
     */
    Runtime &join() noexcept
    {
      pool_.join();
      return *this;
    }

    /**
     * @brief Submit a task for execution through the runtime executor.
     *
     * @param task Task to submit
     * @return bool true if submitted, false otherwise
     */
    [[nodiscard]] bool post(task_type task)
    {
      return executor_.post(std::move(task));
    }

    /**
     * @brief Dispatch a task through the runtime scheduler.
     *
     * @param task Task to dispatch
     * @return bool true if submitted, false otherwise
     */
    [[nodiscard]] bool dispatch(task_type task)
    {
      return scheduler_.dispatch(std::move(task));
    }

    /**
     * @brief Returns mutable access to the runtime configuration.
     *
     * @return config_type& Runtime configuration
     */
    [[nodiscard]] config_type &config() noexcept
    {
      return config_;
    }

    /**
     * @brief Returns const access to the runtime configuration.
     *
     * @return const config_type& Runtime configuration
     */
    [[nodiscard]] const config_type &config() const noexcept
    {
      return config_;
    }

    /**
     * @brief Returns mutable access to the owned thread pool.
     *
     * @return pool_type& Thread pool
     */
    [[nodiscard]] pool_type &pool() noexcept
    {
      return pool_;
    }

    /**
     * @brief Returns const access to the owned thread pool.
     *
     * @return const pool_type& Thread pool
     */
    [[nodiscard]] const pool_type &pool() const noexcept
    {
      return pool_;
    }

    /**
     * @brief Returns mutable access to the runtime executor.
     *
     * @return executor_type& Executor
     */
    [[nodiscard]] executor_type &executor() noexcept
    {
      return executor_;
    }

    /**
     * @brief Returns const access to the runtime executor.
     *
     * @return const executor_type& Executor
     */
    [[nodiscard]] const executor_type &executor() const noexcept
    {
      return executor_;
    }

    /**
     * @brief Returns mutable access to the runtime scheduler.
     *
     * @return scheduler_type& Scheduler
     */
    [[nodiscard]] scheduler_type &scheduler() noexcept
    {
      return scheduler_;
    }

    /**
     * @brief Returns const access to the runtime scheduler.
     *
     * @return const scheduler_type& Scheduler
     */
    [[nodiscard]] const scheduler_type &scheduler() const noexcept
    {
      return scheduler_;
    }

    /**
     * @brief Returns true if the runtime is currently running.
     *
     * @return true if running
     */
    [[nodiscard]] bool running() const noexcept
    {
      return pool_.running();
    }

    /**
     * @brief Returns the number of worker threads in the runtime.
     *
     * @return std::size_t Worker count
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return pool_.size();
    }

    /**
     * @brief Returns the current number of queued tasks.
     *
     * @return std::size_t Queue size
     */
    [[nodiscard]] std::size_t queued_tasks() const noexcept
    {
      return pool_.queued_tasks();
    }

  private:
    config_type config_{};
    pool_type pool_;
    executor_type executor_;
    scheduler_type scheduler_;
  };

} // namespace cnerium::runtime
