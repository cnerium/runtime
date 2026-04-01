#include <cassert>
#include <atomic>
#include <chrono>
#include <thread>

#include <cnerium/runtime/runtime.hpp>

int main()
{
  using namespace cnerium::runtime;

  Runtime runtime;
  runtime.start();

  assert(runtime.running());
  assert(runtime.size() > 0);

  std::atomic<int> counter{0};

  const bool ok1 = runtime.post([&counter]()
                                { ++counter; });

  const bool ok2 = runtime.dispatch([&counter]()
                                    { ++counter; });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  runtime.stop();
  runtime.join();

  assert(ok1);
  assert(ok2);
  assert(counter.load() == 2);

  return 0;
}
