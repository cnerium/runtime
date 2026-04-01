/**
 * @file RuntimeConfig.hpp
 * @brief cnerium::runtime — Runtime configuration
 *
 * @version 0.1.0
 * @author Gaspard Kirira
 * @copyright (c) 2026 Gaspard Kirira
 * @license MIT
 *
 * @details
 * Defines the configuration object used to initialize and control
 * the behavior of the Cnerium Runtime.
 *
 * A RuntimeConfig instance controls:
 *   - number of worker threads
 *   - queue capacity limits
 *   - execution behavior tuning
 *
 * Design goals:
 *   - Simple and explicit configuration
 *   - Safe defaults out of the box
 *   - No dynamic allocation
 *   - Easily extensible for future features
 *
 * Notes:
 *   - Defaults are tuned for general-purpose usage
 *   - Thread count defaults to hardware concurrency
 *   - Queue size 0 means "unbounded"
 *
 * Usage:
 * @code
 *   using namespace cnerium::runtime;
 *
 *   RuntimeConfig config;
 *   config.thread_count = 4;
 *
 *   Runtime runtime(config);
 * @endcode
 */

#pragma once

#include <cstddef>
#include <thread>

namespace cnerium::runtime
{
  /**
   * @brief Configuration object for the runtime.
   */
  struct RuntimeConfig
  {
    /**
     * @brief Number of worker threads.
     *
     * Default:
     *   std::thread::hardware_concurrency()
     */
    std::size_t thread_count{std::thread::hardware_concurrency()};

    /**
     * @brief Maximum number of tasks in the queue.
     *
     * 0 means unbounded queue.
     *
     * Default: 0
     */
    std::size_t max_queue_size{0};

    /**
     * @brief Returns true if the configuration is valid.
     *
     * @return true if valid
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return thread_count > 0;
    }

    /**
     * @brief Reset configuration to default values.
     */
    void reset() noexcept
    {
      thread_count = std::thread::hardware_concurrency();
      max_queue_size = 0;
    }
  };

} // namespace cnerium::runtime
