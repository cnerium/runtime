# cnerium/runtime

Concurrent execution runtime for the Cnerium web framework.

**Header-only. Deterministic. ThreadPool + Executor + Scheduler.**

---

## Download

https://vixcpp.com/registry/pkg/cnerium/runtime

---

## Overview

`cnerium/runtime` is the execution engine of the Cnerium web stack.

It provides:

- thread pool (`ThreadPool`)
- task execution (`Executor`)
- scheduling (`Scheduler`)
- runtime orchestration (`Runtime`)
- server integration (`ServerRunner`)

It is responsible for:

- managing worker threads
- executing tasks concurrently
- distributing work across threads
- orchestrating server execution

It is designed to be:

- minimal
- predictable
- fully deterministic
- easy to extend

---

## Why cnerium/runtime?

Most systems mix:

- concurrency
- execution logic
- networking
- scheduling

This leads to:

- complex threading bugs
- unpredictable behavior
- tight coupling
- difficult scaling

`cnerium/runtime` separates concerns:

- **server** → HTTP logic
- **runtime** → execution strategy
- **executor** → task submission
- **scheduler** → task distribution

Result:

- clean concurrency model
- predictable execution
- scalable architecture

---

## Dependencies

Depends on:

- `cnerium/server`

This ensures:

- clear separation between execution and HTTP logic
- runtime can scale independently from server
- no circular dependencies

---

## Installation

### Using Vix

```bash
vix add @cnerium/runtime
vix install
```

### Manual

```bash
git clone https://github.com/cnerium/runtime.git
```

Add `include/` to your project.

---

## Core Concepts

### ThreadPool

```cpp
ThreadPool pool;
pool.start();
```

Executes tasks across multiple threads.

---

### Executor

```cpp
Executor executor(pool);

executor.post([]()
{
  // work
});
```

Task submission interface.

---

### Scheduler

```cpp
Scheduler scheduler(executor);

scheduler.schedule([]()
{
  // work
});
```

Future-proof scheduling layer.

---

### Runtime

```cpp
Runtime runtime;
runtime.start();
```

High-level runtime facade.

---

### ServerRunner

```cpp
ServerRunner runner(runtime, server);
runner.run();
```

Connects runtime with HTTP server.

---

## Typical Flow

```text
accept → dispatch → thread pool → worker → task execution
```

---

## Example

```cpp
#include <cnerium/runtime/runtime.hpp>

using namespace cnerium::runtime;

int main()
{
  Runtime runtime;
  runtime.start();

  runtime.post([]()
  {
    // work
  });

  runtime.stop();
  runtime.join();
}
```

---

## ThreadPool Example

```cpp
ThreadPool pool;
pool.start();

pool.post([]()
{
  // task
});
```

---

## Executor Example

```cpp
Executor executor(pool);

executor.post([]()
{
  // task
});
```

---

## Runtime + Server Example

```cpp
#include <cnerium/runtime/runtime.hpp>
#include <cnerium/server/server.hpp>

using namespace cnerium::runtime;
using namespace cnerium::server;

int main()
{
  Runtime runtime;

  Server server;

  server.get("/", [](Context &ctx)
  {
    ctx.response().text("Hello from Cnerium");
  });

  ServerRunner runner(runtime, server);
  runner.run();
}
```

---

## Execution Rules

- tasks are executed FIFO
- workers pull tasks from a shared queue
- execution is cooperative (no forced interruption)
- stop() signals workers to terminate
- join() waits for all threads

---

## Complexity

| Operation | Complexity |
|----------|-----------|
| task enqueue | O(1) |
| task dequeue | O(1) |
| task execution | O(n) |
| scheduling | O(1) |

---

## Design Philosophy

- minimal runtime core
- deterministic execution
- explicit concurrency model
- no hidden magic
- composable architecture

---

## Architecture

```text
Task → Queue → Worker → ThreadPool → Executor → Scheduler → Runtime
```

---

## Tests

```bash
vix build
vix test
```

---

## License

MIT License
Copyright (c) Gaspard Kirira

