#include <iostream>
#include <chrono>
#include <thread>

#include <cnerium/runtime/runtime.hpp>

using namespace cnerium::runtime;

int main()
{
  Runtime runtime;
  runtime.start();

  runtime.post([]()
               { std::cout << "[Runtime] Task 1\n"; });

  runtime.dispatch([]()
                   { std::cout << "[Runtime] Task 2\n"; });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  runtime.stop();
  runtime.join();

  std::cout << "Runtime finished\n";
  return 0;
}
