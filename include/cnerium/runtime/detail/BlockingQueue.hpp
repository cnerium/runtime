/**
 * @file BlockingQueue.hpp
 * @brief cnerium::runtime::detail — Thread-safe blocking queue
 *
 * @version 0.1.0
 *
 * @details
 * Provides a thread-safe FIFO queue with blocking and non-blocking
 * operations, used as the core communication primitive between
 * producers (Executor) and consumers (Worker threads).
 *
 * Responsibilities:
 *   - store tasks in FIFO order
 *   - allow multiple producers and consumers
 *   - block consumers when queue is empty
 *   - wake consumers when new tasks arrive
 *   - support graceful shutdown via StopToken
 *
 * Design goals:
 *   - Header-only
 *   - Strong exception safety
 *   - Minimal locking scope
 *   - Deterministic behavior
 *
 * Notes:
 *   - Uses std::mutex + std::condition_variable
 *   - stop() wakes all waiting threads
 *   - wait_and_pop() returns false when stopped
 *
 * Usage:
 * @code
 *   BlockingQueue<Task> queue;
 *
 *   queue.push(task);
 *
 *   Task t;
 *   if (queue.wait_and_pop(t))
 *   {
 *     t();
 *   }
 * @endcode
 */

#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <utility>

#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/detail/StopToken.hpp>

namespace cnerium::runtime::detail
{
  template <typename T>
  class BlockingQueue
  {
  public:
    using value_type = T;

    BlockingQueue() = default;

    BlockingQueue(const BlockingQueue &) = delete;
    BlockingQueue &operator=(const BlockingQueue &) = delete;

    /**
     * @brief Push a new item into the queue.
     *
     * Notifies one waiting thread.
     */
    void push(T value)
    {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stopped_)
        {
          return;
        }
        queue_.push(std::move(value));
      }
      cv_.notify_one();
    }

    /**
     * @brief Try to pop an item without blocking.
     *
     * @return true if an item was retrieved
     */
    [[nodiscard]] bool try_pop(T &out)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (queue_.empty())
      {
        return false;
      }

      out = std::move(queue_.front());
      queue_.pop();
      return true;
    }

    /**
     * @brief Wait until an item is available or stop is requested.
     *
     * @return true if an item was retrieved, false if stopped
     */
    [[nodiscard]] bool wait_and_pop(T &out)
    {
      std::unique_lock<std::mutex> lock(mutex_);

      cv_.wait(lock, [this]()
               { return stopped_ || !queue_.empty(); });

      if (stopped_ && queue_.empty())
      {
        return false;
      }

      out = std::move(queue_.front());
      queue_.pop();
      return true;
    }

    /**
     * @brief Stop the queue.
     *
     * Wakes all waiting threads and prevents further pushes.
     */
    void stop() noexcept
    {
      {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
      }
      cv_.notify_all();
    }

    /**
     * @brief Returns true if the queue is stopped.
     */
    [[nodiscard]] bool stopped() const noexcept
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return stopped_;
    }

    /**
     * @brief Returns current size of the queue.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.size();
    }

    /**
     * @brief Returns true if the queue is empty.
     */
    [[nodiscard]] bool empty() const noexcept
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.empty();
    }

  private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    bool stopped_{false};
  };

} // namespace cnerium::runtime::detail
