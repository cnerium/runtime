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

  Executor executor(pool);
  assert(executor.valid());

  std::atomic<int> counter{0};

  const bool ok1 = executor.post([&counter]()
                                 { ++counter; });

  const bool ok2 = executor.dispatch([&counter]()
                                     { ++counter; });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  pool.stop();
  pool.join();

  assert(ok1);
  assert(ok2);
  assert(counter.load() == 2);

  return 0;
}
