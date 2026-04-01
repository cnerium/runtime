#include <cassert>

#include <cnerium/runtime/runtime.hpp>

int main()
{
  using namespace cnerium::runtime;

  RuntimeConfig config;
  assert(config.valid());

  Task task = []() {};
  assert(static_cast<bool>(task));

  Runtime runtime;
  assert(runtime.size() > 0);

  return 0;
}
