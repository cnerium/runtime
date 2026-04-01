#include <iostream>
#include <chrono>
#include <thread>

#include <cnerium/runtime/runtime.hpp>

using namespace cnerium::runtime;

int main()
{
  ThreadPool pool;
  pool.start();

  Executor executor(pool);

  executor.post([]()
                { std::cout << "[Executor] Task executed\n"; });

  executor.dispatch([]()
                    { std::cout << "[Executor] Dispatch executed\n"; });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  pool.stop();
  pool.join();

  return 0;
}
