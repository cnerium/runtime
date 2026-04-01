/**
 * @file ThreadName.hpp
 * @brief cnerium::runtime::detail — Thread naming utilities
 *
 * @version 0.1.0
 *
 * @details
 * Provides small cross-platform helpers to assign human-readable
 * names to threads for debugging and profiling purposes.
 *
 * Responsibilities:
 *   - set the current thread name
 *   - optionally get the current thread name (best effort)
 *
 * Design goals:
 *   - Header-only
 *   - No hard dependency on a specific platform
 *   - Graceful fallback when unsupported
 *
 * Notes:
 *   - Linux: pthread_setname_np (limited to 16 chars incl. null)
 *   - macOS: pthread_setname_np (different signature)
 *   - Windows: SetThreadDescription (UTF-16)
 *   - If unsupported, functions become no-ops
 */

#pragma once

#include <string_view>

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

namespace cnerium::runtime::detail
{
  /**
   * @brief Set the name of the current thread.
   *
   * @param name Thread name (may be truncated depending on platform)
   */
  inline void set_thread_name(std::string_view name) noexcept
  {
#if defined(__linux__)
    // Linux: max 16 chars including null terminator
    char buf[16]{};
    const std::size_t n = (name.size() < 15) ? name.size() : 15;
    for (std::size_t i = 0; i < n; ++i)
      buf[i] = name[i];
    buf[n] = '\0';
    pthread_setname_np(pthread_self(), buf);

#elif defined(__APPLE__)
    // macOS: current thread only
    char buf[64]{};
    const std::size_t n = (name.size() < 63) ? name.size() : 63;
    for (std::size_t i = 0; i < n; ++i)
      buf[i] = name[i];
    buf[n] = '\0';
    pthread_setname_np(buf);

#elif defined(_WIN32)
    // Windows 10+: SetThreadDescription
    int len = static_cast<int>(name.size());
    if (len > 0)
    {
      // convert to wide string
      wchar_t wbuf[64]{};
      const int max = (len < 63) ? len : 63;
      for (int i = 0; i < max; ++i)
        wbuf[i] = static_cast<wchar_t>(name[i]);
      wbuf[max] = L'\0';

      using SetThreadDescriptionFn = HRESULT(WINAPI *)(HANDLE, PCWSTR);
      static auto fn = reinterpret_cast<SetThreadDescriptionFn>(
          GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetThreadDescription"));

      if (fn)
        fn(GetCurrentThread(), wbuf);
    }
#else
    (void)name;
#endif
  }

} // namespace cnerium::runtime::detail
