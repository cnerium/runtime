/**
 * @file ThreadPool.hpp
 * @brief cnerium::runtime — Thread pool execution engine
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the ThreadPool class used by the Cnerium Runtime to execute
 * submitted tasks concurrently across a fixed number of worker threads.
 *
 * A ThreadPool:
 *   - owns a shared task queue
 *   - owns a set of Worker objects
 *   - starts and stops worker threads
 *   - accepts tasks for execution
 *
 * Responsibilities:
 *   - create workers from runtime configuration
 *   - manage worker lifecycle
 *   - enqueue executable tasks
 *   - expose simple pool state inspection
 *
 * Design goals:
 *   - Header-only
 *   - Deterministic lifecycle
 *   - Clear ownership model
 *   - Easy integration with Executor and Runtime
 *
 * Notes:
 *   - The pool uses one shared FIFO task queue
 *   - stop() requests worker shutdown and stops the queue
 *   - queued tasks may remain unexecuted after stop()
 *   - start() is idempotent for an already running pool
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   ThreadPool pool;
 *   pool.start();
 *
 *   pool.post([]()
 *   {
 *     // work
 *   });
 *
 *   pool.stop();
 *   pool.join();
 * @endcode
 */

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include <cnerium/runtime/RuntimeConfig.hpp>
#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/Worker.hpp>
#include <cnerium/runtime/detail/ConcurrencyUtils.hpp>
#include <cnerium/runtime/detail/TaskQueue.hpp>

namespace cnerium::runtime
{
  /**
   * @brief Fixed-size thread pool executing tasks from a shared queue.
   */
  class ThreadPool
  {
  public:
    using config_type = cnerium::runtime::RuntimeConfig;
    using task_type = cnerium::runtime::Task;
    using queue_type = cnerium::runtime::detail::TaskQueue;
    using worker_type = cnerium::runtime::Worker;
    using worker_storage_type = std::vector<worker_type>;

    /// @brief Default constructor using default runtime configuration.
    ThreadPool()
        : ThreadPool(config_type{})
    {
    }

    /**
     * @brief Construct a thread pool with explicit configuration.
     *
     * @param config Runtime configuration
     */
    explicit ThreadPool(config_type config)
        : config_(std::move(config))
    {
      normalize_config();
      build_workers();
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    /**
     * @brief Destructor.
     *
     * Stops and joins all worker threads if needed.
     */
    ~ThreadPool()
    {
      stop();
      join();
    }

    /**
     * @brief Start all worker threads.
     *
     * Safe to call multiple times. If already running, this is a no-op.
     */
    void start()
    {
      if (running_)
      {
        return;
      }

      if (workers_.empty())
      {
        normalize_config();
        build_workers();
      }

      for (auto &worker : workers_)
      {
        worker.start();
      }

      running_ = true;
    }

    /**
     * @brief Request cooperative stop of the pool.
     *
     * Stops the shared queue and requests every worker to stop.
     */
    void stop() noexcept
    {
      if (!running_ && queue_.stopped())
      {
        return;
      }

      queue_.stop();

      for (auto &worker : workers_)
      {
        worker.stop();
      }

      running_ = false;
    }

    /**
     * @brief Join all worker threads.
     */
    void join() noexcept
    {
      for (auto &worker : workers_)
      {
        worker.join();
      }
    }

    /**
     * @brief Enqueue a task for execution.
     *
     * If the pool has been stopped, the task is ignored by the queue.
     *
     * @param task Task to enqueue
     */
    void post(task_type task)
    {
      queue_.push(std::move(task));
    }

    /**
     * @brief Returns mutable access to the underlying task queue.
     *
     * @return queue_type& Shared queue
     */
    [[nodiscard]] queue_type &queue() noexcept
    {
      return queue_;
    }

    /**
     * @brief Returns const access to the underlying task queue.
     *
     * @return const queue_type& Shared queue
     */
    [[nodiscard]] const queue_type &queue() const noexcept
    {
      return queue_;
    }

    /**
     * @brief Returns mutable access to the worker storage.
     *
     * @return worker_storage_type& Worker list
     */
    [[nodiscard]] worker_storage_type &workers() noexcept
    {
      return workers_;
    }

    /**
     * @brief Returns const access to the worker storage.
     *
     * @return const worker_storage_type& Worker list
     */
    [[nodiscard]] const worker_storage_type &workers() const noexcept
    {
      return workers_;
    }

    /**
     * @brief Returns mutable access to the runtime configuration.
     *
     * @return config_type& Pool configuration
     */
    [[nodiscard]] config_type &config() noexcept
    {
      return config_;
    }

    /**
     * @brief Returns const access to the runtime configuration.
     *
     * @return const config_type& Pool configuration
     */
    [[nodiscard]] const config_type &config() const noexcept
    {
      return config_;
    }

    /**
     * @brief Returns true if the pool is currently running.
     *
     * @return true if running
     */
    [[nodiscard]] bool running() const noexcept
    {
      return running_;
    }

    /**
     * @brief Returns the number of worker threads configured in the pool.
     *
     * @return std::size_t Worker count
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return workers_.size();
    }

    /**
     * @brief Returns true if the pool has no workers.
     *
     * @return true if empty
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return workers_.empty();
    }

    /**
     * @brief Returns the current number of queued tasks.
     *
     * @return std::size_t Queue size
     */
    [[nodiscard]] std::size_t queued_tasks() const noexcept
    {
      return queue_.size();
    }

  private:
    /**
     * @brief Normalize runtime configuration for safe worker creation.
     */
    void normalize_config() noexcept
    {
      config_.thread_count =
          cnerium::runtime::detail::compute_worker_count(config_.thread_count);
    }

    /**
     * @brief Build worker objects from the current configuration.
     */
    void build_workers()
    {
      workers_.clear();
      workers_.reserve(config_.thread_count);

      for (std::size_t i = 0; i < config_.thread_count; ++i)
      {
        workers_.emplace_back(i, queue_);
      }
    }

  private:
    config_type config_{};
    queue_type queue_{};
    worker_storage_type workers_{};
    bool running_{false};
  };

} // namespace cnerium::runtime
