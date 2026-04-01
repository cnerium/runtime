/**
 * @file StopToken.hpp
 * @brief cnerium::runtime::detail — Cooperative stop signal
 *
 * @version 0.1.0
 *
 * @details
 * Defines a lightweight cooperative stop mechanism used by the runtime
 * to signal workers, queues, and loops to terminate gracefully.
 *
 * Responsibilities:
 *   - provide a shared stop flag
 *   - allow multiple threads to observe stop requests
 *   - support safe concurrent access without locks
 *
 * Design goals:
 *   - Header-only
 *   - Lock-free (atomic-based)
 *   - Very small footprint
 *   - Internal use only (detail namespace)
 *
 * Notes:
 *   - This is NOT std::stop_token
 *   - Simpler and tailored for the runtime
 *   - Can be shared across workers and queues
 */

#pragma once

#include <atomic>

namespace cnerium::runtime::detail
{
  /**
   * @brief Cooperative stop signal shared across threads.
   *
   * Allows producers and consumers to detect when shutdown
   * has been requested.
   */
  class StopToken
  {
  public:
    /**
     * @brief Request stop.
     *
     * Sets the internal flag to true.
     * Safe to call from any thread.
     */
    void request_stop() noexcept
    {
      stop_.store(true, std::memory_order_release);
    }

    /**
     * @brief Returns true if stop has been requested.
     */
    [[nodiscard]] bool stop_requested() const noexcept
    {
      return stop_.load(std::memory_order_acquire);
    }

    /**
     * @brief Reset stop state (for reuse).
     *
     * Should only be called when no workers are running.
     */
    void reset() noexcept
    {
      stop_.store(false, std::memory_order_release);
    }

  private:
    std::atomic<bool> stop_{false};
  };

} // namespace cnerium::runtime::detail
