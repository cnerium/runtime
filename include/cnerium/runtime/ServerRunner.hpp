/**
 * @file ServerRunner.hpp
 * @brief cnerium::runtime — Runtime + Server integration
 *
 * @version 0.1.0
 *
 * @details
 * Connects the Runtime (thread pool) with the Server (HTTP engine).
 *
 * ServerRunner:
 *   - owns a reference to Runtime
 *   - owns a reference to Server
 *   - accepts incoming connections
 *   - dispatches them to the runtime thread pool
 *
 * Responsibilities:
 *   - bridge server execution with runtime concurrency
 *   - offload connection handling to workers
 *   - keep server single-thread compatible
 *
 * Design goals:
 *   - Header-only
 *   - No ownership of runtime/server
 *   - Clear separation of concerns
 *
 * Notes:
 *   - runtime controls execution
 *   - server handles HTTP logic
 *   - this class wires both together
 */
#pragma once

#include <memory>
#include <utility>

#include <cnerium/runtime/Runtime.hpp>
#include <cnerium/runtime/Executor.hpp>

#include <cnerium/server/Server.hpp>
#include <cnerium/server/net/TcpListener.hpp>
#include <cnerium/server/net/TcpConnection.hpp>

namespace cnerium::runtime
{
  class ServerRunner
  {
  public:
    using runtime_type = cnerium::runtime::Runtime;
    using executor_type = cnerium::runtime::Executor;
    using server_type = cnerium::server::Server;
    using listener_type = cnerium::server::net::TcpListener;
    using connection_type = cnerium::server::net::TcpConnection;

    ServerRunner(runtime_type &runtime, server_type &server)
        : runtime_(&runtime),
          executor_(&runtime.executor()),
          server_(&server)
    {
    }

    ServerRunner(const ServerRunner &) = delete;
    ServerRunner &operator=(const ServerRunner &) = delete;

    void run()
    {
      if (!runtime_ || !server_ || !executor_)
      {
        return;
      }

      runtime_->start();

      listener_type listener(*server_);
      listener.start();

      while (listener.running())
      {
        auto connection = std::make_shared<connection_type>(listener.accept());

        executor_->post(
            [connection]() mutable
            {
              connection->process();
            });
      }
    }

  private:
    runtime_type *runtime_{nullptr};
    executor_type *executor_{nullptr};
    server_type *server_{nullptr};
  };

} // namespace cnerium::runtime
