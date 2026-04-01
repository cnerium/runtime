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
  Scheduler scheduler(executor);

  assert(scheduler.valid());

  std::atomic<int> counter{0};

  const bool ok1 = scheduler.schedule([&counter]()
                                      { ++counter; });

  const bool ok2 = scheduler.post([&counter]()
                                  { ++counter; });

  const bool ok3 = scheduler.dispatch([&counter]()
                                      { ++counter; });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  pool.stop();
  pool.join();

  assert(ok1);
  assert(ok2);
  assert(ok3);
  assert(counter.load() == 3);

  return 0;
}
