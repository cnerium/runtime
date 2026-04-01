/**
 * @file TaskQueue.hpp
 * @brief cnerium::runtime::detail — Runtime task queue abstraction
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the task queue abstraction used internally by the runtime
 * to store and retrieve executable tasks.
 *
 * TaskQueue is a thin semantic wrapper above BlockingQueue<Task>.
 * It provides a clearer runtime-oriented API while preserving the
 * deterministic FIFO blocking behavior of the underlying queue.
 *
 * Responsibilities:
 *   - store runtime tasks
 *   - expose blocking and non-blocking pop operations
 *   - expose queue lifecycle helpers
 *   - provide a runtime-specific naming layer over BlockingQueue
 *
 * Design goals:
 *   - Header-only
 *   - Minimal abstraction cost
 *   - Clear runtime semantics
 *   - Easy integration with Worker and ThreadPool
 *
 * Notes:
 *   - FIFO ordering is preserved
 *   - stop() wakes all blocked consumers
 *   - once stopped, new tasks are ignored by the underlying queue
 *
 * Usage:
 * @code
 *   cnerium::runtime::detail::TaskQueue queue;
 *
 *   queue.push([]()
 *   {
 *     // work
 *   });
 *
 *   cnerium::runtime::Task task;
 *   if (queue.wait_and_pop(task))
 *   {
 *     task();
 *   }
 * @endcode
 */

#pragma once

#include <cstddef>
#include <utility>

#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/detail/BlockingQueue.hpp>

namespace cnerium::runtime::detail
{
  /**
   * @brief Runtime-specific FIFO queue of executable tasks.
   *
   * This class provides a semantic wrapper around BlockingQueue<Task>.
   */
  class TaskQueue
  {
  public:
    using task_type = cnerium::runtime::Task;
    using queue_type = cnerium::runtime::detail::BlockingQueue<task_type>;

    /// @brief Default constructor.
    TaskQueue() = default;

    TaskQueue(const TaskQueue &) = delete;
    TaskQueue &operator=(const TaskQueue &) = delete;

    /**
     * @brief Push a new task into the queue.
     *
     * @param task Task to enqueue
     */
    void push(task_type task)
    {
      queue_.push(std::move(task));
    }

    /**
     * @brief Try to pop one task without blocking.
     *
     * @param out Destination task
     * @return true if a task was retrieved
     */
    [[nodiscard]] bool try_pop(task_type &out)
    {
      return queue_.try_pop(out);
    }

    /**
     * @brief Wait until one task is available or the queue is stopped.
     *
     * @param out Destination task
     * @return true if a task was retrieved, false if stopped
     */
    [[nodiscard]] bool wait_and_pop(task_type &out)
    {
      return queue_.wait_and_pop(out);
    }

    /**
     * @brief Stop the queue and wake all waiting consumers.
     */
    void stop() noexcept
    {
      queue_.stop();
    }

    /**
     * @brief Returns true if the queue has been stopped.
     *
     * @return true if stopped
     */
    [[nodiscard]] bool stopped() const noexcept
    {
      return queue_.stopped();
    }

    /**
     * @brief Returns the number of queued tasks.
     *
     * @return std::size_t Current queue size
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      return queue_.size();
    }

    /**
     * @brief Returns true if the queue is empty.
     *
     * @return true if empty
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return queue_.empty();
    }

  private:
    queue_type queue_{};
  };

} // namespace cnerium::runtime::detail
