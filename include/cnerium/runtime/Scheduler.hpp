/**
 * @file Scheduler.hpp
 * @brief cnerium::runtime — Scheduling facade for task execution
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the Scheduler class used by the Cnerium Runtime to provide
 * a small scheduling layer above the Executor.
 *
 * A Scheduler:
 *   - references an existing Executor
 *   - forwards tasks for execution
 *   - centralizes scheduling decisions
 *
 * Responsibilities:
 *   - expose a scheduling API
 *   - validate scheduling state
 *   - act as the future extension point for scheduling strategies
 *
 * Design goals:
 *   - Header-only
 *   - Lightweight
 *   - Non-owning
 *   - Easy to extend later
 *
 * Notes:
 *   - For now, scheduling is simple FIFO forwarding through Executor
 *   - Future versions may add:
 *       - delayed tasks
 *       - priorities
 *       - affinity
 *       - round-robin worker selection
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   ThreadPool pool;
 *   pool.start();
 *
 *   Executor executor(pool);
 *   Scheduler scheduler(executor);
 *
 *   scheduler.schedule([]()
 *   {
 *     // work
 *   });
 * @endcode
 */

#pragma once

#include <utility>

#include <cnerium/runtime/Executor.hpp>
#include <cnerium/runtime/Task.hpp>

namespace cnerium::runtime
{
  /**
   * @brief Lightweight scheduling facade above an Executor.
   */
  class Scheduler
  {
  public:
    using task_type = cnerium::runtime::Task;
    using executor_type = cnerium::runtime::Executor;

    /// @brief Default constructor.
    Scheduler() = default;

    /**
     * @brief Construct a scheduler bound to an existing executor.
     *
     * @param executor Target executor
     */
    explicit Scheduler(executor_type &executor) noexcept
        : executor_(&executor)
    {
    }

    Scheduler(const Scheduler &) = default;
    Scheduler &operator=(const Scheduler &) = default;
    Scheduler(Scheduler &&) noexcept = default;
    Scheduler &operator=(Scheduler &&) noexcept = default;

    /**
     * @brief Bind the scheduler to an executor.
     *
     * @param executor Target executor
     * @return Scheduler& Self
     */
    Scheduler &bind(executor_type &executor) noexcept
    {
      executor_ = &executor;
      return *this;
    }

    /**
     * @brief Remove the currently bound executor.
     *
     * @return Scheduler& Self
     */
    Scheduler &reset() noexcept
    {
      executor_ = nullptr;
      return *this;
    }

    /**
     * @brief Returns true if the scheduler is bound to a valid executor.
     *
     * @return true if valid
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return executor_ != nullptr;
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
     * @brief Returns the bound executor pointer.
     *
     * @return executor_type* Executor pointer, or nullptr
     */
    [[nodiscard]] executor_type *executor() noexcept
    {
      return executor_;
    }

    /**
     * @brief Returns the bound executor pointer.
     *
     * @return const executor_type* Executor pointer, or nullptr
     */
    [[nodiscard]] const executor_type *executor() const noexcept
    {
      return executor_;
    }

    /**
     * @brief Schedule a task for execution.
     *
     * For now, this forwards directly to Executor::post().
     *
     * @param task Task to schedule
     * @return bool true if scheduled, false otherwise
     */
    [[nodiscard]] bool schedule(task_type task)
    {
      if (!executor_)
      {
        return false;
      }

      return executor_->post(std::move(task));
    }

    /**
     * @brief Alias of schedule().
     *
     * @param task Task to schedule
     * @return bool true if scheduled, false otherwise
     */
    [[nodiscard]] bool post(task_type task)
    {
      return schedule(std::move(task));
    }

    /**
     * @brief Dispatch a task through the scheduler.
     *
     * For now, this behaves the same as schedule().
     *
     * @param task Task to dispatch
     * @return bool true if scheduled, false otherwise
     */
    [[nodiscard]] bool dispatch(task_type task)
    {
      return schedule(std::move(task));
    }

  private:
    executor_type *executor_{nullptr};
  };

} // namespace cnerium::runtime
