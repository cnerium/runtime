/**
 * @file Worker.hpp
 * @brief cnerium::runtime — Worker thread execution unit
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the Worker class used internally by the runtime to execute
 * queued tasks on a dedicated thread.
 *
 * A Worker:
 *   - owns one std::thread
 *   - pulls tasks from a shared TaskQueue
 *   - executes tasks sequentially
 *   - stops cooperatively when requested
 *
 * Responsibilities:
 *   - run a task execution loop
 *   - fetch tasks from the shared queue
 *   - execute tasks safely
 *   - support start, stop, and join lifecycle operations
 *
 * Design goals:
 *   - Header-only
 *   - Small and deterministic
 *   - Clear ownership model
 *   - Easy integration with ThreadPool
 *
 * Notes:
 *   - A Worker does not own the queue
 *   - A Worker executes tasks synchronously on its own thread
 *   - Exceptions thrown by tasks are swallowed to keep the worker alive
 *   - stop() is cooperative and does not forcibly interrupt a running task
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   detail::TaskQueue queue;
 *   Worker worker(0, queue);
 *
 **/

#pragma once

#include <cstddef>
#include <string>
#include <thread>
#include <utility>

#include <cnerium/runtime/Task.hpp>
#include <cnerium/runtime/detail/StopToken.hpp>
#include <cnerium/runtime/detail/TaskQueue.hpp>
#include <cnerium/runtime/detail/ThreadName.hpp>

namespace cnerium::runtime
{
  /**
   * @brief Dedicated worker thread consuming tasks from a shared queue.
   */
  class Worker
  {
  public:
    using task_type = cnerium::runtime::Task;
    using queue_type = cnerium::runtime::detail::TaskQueue;
    using stop_token_type = cnerium::runtime::detail::StopToken;

    /**
     * @brief Construct a worker bound to a shared task queue.
     *
     * @param id Worker identifier
     * @param queue Shared task queue
     */
    Worker(std::size_t id, queue_type &queue)
        : id_(id),
          queue_(&queue)
    {
    }

    Worker(const Worker &) = delete;
    Worker &operator=(const Worker &) = delete;

    Worker(Worker &&other) noexcept
        : id_(other.id_),
          queue_(other.queue_),
          thread_(std::move(other.thread_)),
          running_(other.running_)
    {
      if (other.stop_token_.stop_requested())
      {
        stop_token_.request_stop();
      }

      other.queue_ = nullptr;
      other.running_ = false;
      other.stop_token_.reset();
    }

    Worker &operator=(Worker &&other) noexcept
    {
      if (this != &other)
      {
        stop();
        join();

        id_ = other.id_;
        queue_ = other.queue_;
        thread_ = std::move(other.thread_);
        running_ = other.running_;

        if (other.stop_token_.stop_requested())
        {
          stop_token_.request_stop();
        }
        else
        {
          stop_token_.reset();
        }

        other.queue_ = nullptr;
        other.running_ = false;
        other.stop_token_.reset();
      }

      return *this;
    }

    /**
     * @brief Destructor.
     *
     * Requests stop and joins the worker thread if needed.
     */
    ~Worker()
    {
      stop();
      join();
    }

    /**
     * @brief Returns the worker identifier.
     *
     * @return std::size_t Worker id
     */
    [[nodiscard]] std::size_t id() const noexcept
    {
      return id_;
    }

    /**
     * @brief Returns true if the worker has an associated queue.
     *
     * @return true if valid
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return queue_ != nullptr;
    }

    /**
     * @brief Returns true if the worker thread is running.
     *
     * @return true if running
     */
    [[nodiscard]] bool running() const noexcept
    {
      return running_;
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
     * @brief Start the worker thread.
     *
     * Safe to call once per worker lifetime before stop/join cycle.
     */
    void start()
    {
      if (!valid() || running_)
      {
        return;
      }

      stop_token_.reset();
      running_ = true;

      thread_ = std::thread([this]()
                            { run(); });
    }

    /**
     * @brief Request cooperative stop.
     *
     * Does not interrupt a currently running task.
     */
    void stop() noexcept
    {
      stop_token_.request_stop();
    }

    /**
     * @brief Join the worker thread if joinable.
     */
    void join() noexcept
    {
      if (thread_.joinable())
      {
        thread_.join();
      }

      running_ = false;
    }

    /**
     * @brief Returns true if stop has been requested.
     *
     * @return true if stop requested
     */
    [[nodiscard]] bool stop_requested() const noexcept
    {
      return stop_token_.stop_requested();
    }

  private:
    /**
     * @brief Main worker execution loop.
     */
    void run()
    {
      cnerium::runtime::detail::set_thread_name(thread_name());

      while (!stop_token_.stop_requested())
      {
        task_type task;
        if (!queue_->wait_and_pop(task))
        {
          break;
        }

        if (!task)
        {
          continue;
        }

        try
        {
          task();
        }
        catch (...)
        {
          // Keep worker alive even if a task fails.
        }
      }

      running_ = false;
    }

    /**
     * @brief Build a small debug-friendly thread name.
     *
     * @return std::string Thread name
     */
    [[nodiscard]] std::string thread_name() const
    {
      return "cnerium-w" + std::to_string(id_);
    }

  private:
    std::size_t id_{0};
    queue_type *queue_{nullptr};
    stop_token_type stop_token_{};
    std::thread thread_{};
    bool running_{false};
  };

} // namespace cnerium::runtime
