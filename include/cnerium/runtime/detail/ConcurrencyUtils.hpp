/**
 * @file ConcurrencyUtils.hpp
 * @brief cnerium::runtime::detail — Low-level concurrency helpers
 *
 * @version 0.1.0
 *
 * @details
 * Small utility functions used internally by the runtime to:
 *   - normalize thread counts
 *   - provide safe fallbacks for hardware concurrency
 *   - perform lightweight concurrency-related calculations
 *
 * Design goals:
 *   - Header-only
 *   - Zero overhead
 *   - No heavy dependencies
 *   - Internal use only (detail namespace)
 */

#pragma once

#include <cstddef>
#include <thread>
#include <algorithm>

namespace cnerium::runtime::detail
{
  /**
   * @brief Returns the number of hardware threads, with safe fallback.
   *
   * std::thread::hardware_concurrency() may return 0,
   * so we guarantee at least 1.
   */
  [[nodiscard]] inline std::size_t hardware_threads() noexcept
  {
    const std::size_t n = std::thread::hardware_concurrency();
    return n == 0 ? 1 : n;
  }

  /**
   * @brief Clamp a thread count to a safe range.
   *
   * Ensures:
   *   - at least 1 thread
   *   - no overflow or invalid values
   */
  [[nodiscard]] inline std::size_t clamp_thread_count(std::size_t n) noexcept
  {
    return std::max<std::size_t>(1, n);
  }

  /**
   * @brief Normalize a thread count using hardware defaults if needed.
   *
   * If n == 0 → fallback to hardware_threads()
   */
  [[nodiscard]] inline std::size_t normalize_thread_count(std::size_t n) noexcept
  {
    if (n == 0)
    {
      return hardware_threads();
    }

    return clamp_thread_count(n);
  }

  /**
   * @brief Compute an initial worker count based on configuration.
   *
   * This is the function you should use when creating the thread pool.
   */
  [[nodiscard]] inline std::size_t compute_worker_count(std::size_t requested) noexcept
  {
    return normalize_thread_count(requested);
  }

  /**
   * @brief Simple yield hint for busy-wait loops.
   *
   * Useful for spin-wait or cooperative scheduling.
   */
  inline void cpu_relax() noexcept
  {
#if defined(__x86_64__) || defined(_M_X64)
    __asm__ __volatile__("pause");
#else
    std::this_thread::yield();
#endif
  }

} // namespace cnerium::runtime::detail
