#include <cassert>
#include <atomic>
#include <chrono>
#include <thread>

#include <cnerium/runtime/runtime.hpp>

int main()
{
  using namespace cnerium::runtime;

  ThreadPool pool;
  pool.start();

  std::atomic<int> counter{0};

  for (int i = 0; i < 8; ++i)
  {
    pool.post([&counter]()
              { ++counter; });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  pool.stop();
  pool.join();

  assert(counter.load() == 8);
  return 0;
}
