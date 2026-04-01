#include <cassert>

#include <cnerium/runtime/runtime.hpp>
#include <cnerium/server/server.hpp>

int main()
{
  using namespace cnerium::runtime;
  using namespace cnerium::server;

  Runtime runtime;
  Server server;

  server.get("/", [](Context &ctx)
             { ctx.response().text("ok"); });

  ServerRunner runner(runtime, server);

  assert(true);
  return 0;
}
