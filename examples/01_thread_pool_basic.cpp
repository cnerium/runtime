#include <iostream>
#include <chrono>
#include <thread>

#include <cnerium/runtime/runtime.hpp>

using namespace cnerium::runtime;

int main()
{
  ThreadPool pool;
  pool.start();

  for (int i = 0; i < 5; ++i)
  {
    pool.post([i]()
              {
                std::cout << "[ThreadPool] Task " << i << " running\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  pool.stop();
  pool.join();

  std::cout << "ThreadPool finished\n";
  return 0;
}
