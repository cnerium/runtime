#include <iostream>

#include <cnerium/runtime/runtime.hpp>
#include <cnerium/server/server.hpp>

using namespace cnerium::runtime;
using namespace cnerium::server;

int main()
{
  Runtime runtime;

  Server server;

  server.get("/", [](Context &ctx)
             { ctx.response().text("Hello from runtime + server"); });

  std::cout << "Server running on http://127.0.0.1:8080\n";

  ServerRunner runner(runtime, server);
  runner.run();

  return 0;
}
