# anyhow-cpp&ensp;¯\\\_(°ペ)\_/¯

Header-only C++20 result type with propagating stacktraced errors, inspired by Rust's [`anyhow`](https://github.com/dtolnay/anyhow).

[![License](https://img.shields.io/github/license/MihaiStreames/anyhow-cpp?label=license)](LICENSE)

```cpp
anyhow::Expected<Config> load(std::string_view path) {
    if (path.empty()) return anyhow::fail("path is empty", "io");

    std::string text;
    ANYHOW_TRY_ASSIGN(text, read_file(path));

    return parse(text);
}
```

## Install

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  anyhow-cpp
  GIT_REPOSITORY https://github.com/MihaiStreames/anyhow-cpp.git
  GIT_TAG        v0.1.0
)
FetchContent_MakeAvailable(anyhow-cpp)

target_link_libraries(your-target PRIVATE anyhow::anyhow)
```

### Git submodule

```sh
git submodule add https://github.com/MihaiStreames/anyhow-cpp.git external/anyhow-cpp
```

```cmake
add_subdirectory(external/anyhow-cpp)
target_link_libraries(your-target PRIVATE anyhow::anyhow)
```

## Usage

Include everything at once with `anyhow.hpp`, or pull in individual headers as needed.

| Header            | Provides                                              |
| ----------------- | ----------------------------------------------------- |
| `anyhow.hpp`      | Includes all headers below                            |
| `expected.hpp`    | `Expected<T>`, `Expected<void>`                       |
| `failure.hpp`     | `Failure`, `Unexpected`, `fail()`                     |
| `macros.hpp`      | `ANYHOW_TRY`, `ANYHOW_TRY_ASSIGN`, `ANYHOW_TRY_CATCH` |
| `scope_guard.hpp` | `ScopeGuard`                                          |

Define `ANYHOW_SHORT_MACROS` before including `macros.hpp` to enable the short aliases `TRY`, `TRY_ASSIGN`, `TRY_CATCH`.

```cpp
#include <anyhow.hpp>
```

Use `Expected<T>` as the return type of any fallible function. Return `anyhow::fail(message, domain)` on failure, or wrap a value in `Expected<T>{value}` on success.

```cpp
anyhow::Expected<int> parse(std::string_view s) {
    if (s.empty()) return anyhow::fail("empty input", "parse");
    return 42;
}
```

### Propagation

Use `ANYHOW_TRY_ASSIGN` to unwrap a value or propagate the failure up. Each macro captures the current call site via `std::source_location` and pushes it onto the frame buffer, building a trace as the error travels up the stack.

```cpp
anyhow::Expected<std::string> process(std::string_view s) {
    int n = 0;
    ANYHOW_TRY_ASSIGN(n, parse(s));
    ANYHOW_TRY(validate(n));
    return std::to_string(n);
}
```

```cpp
auto r = process("");
if (r.failed()) {
    auto& f = r.failure();
    std::println("error [{}]: {}", f.domain(), f.message());
    for (size_t i = 0; i < f.count; i++)
        std::println("  at {} ({}:{})", f.frames[i].function, f.frames[i].file, f.frames[i].line);
}
```

```console
error [parse]: empty input
  at parse(std::string_view) (src/main.cpp:12)
  at process(std::string_view) (src/main.cpp:19)
  at main() (src/main.cpp:34)
```

### Chaining

`map`, `and_then`, and `value_or` are available for functional chaining on results.

```cpp
auto result = parse("21")
    .map([](int v) { return v * 2; })
    .and_then([](int v) -> anyhow::Expected<std::string> {
        return std::to_string(v);
    });
```

`ScopeGuard` runs a callable on scope exit. Call `release()` to cancel.

```cpp
auto guard = anyhow::ScopeGuard{[&] { cleanup(); }};
```

Override the frame buffer depth at compile time (default: `16`). When full, oldest frames are evicted. Must be defined before any anyhow include.

```cpp
#define ANYHOW_MAX_FRAMES 32
#include <anyhow.hpp>
```

## vs Rust's anyhow

| Feature                                   | Status                                   |
| ----------------------------------------- | ---------------------------------------- |
| `Expected<T>` / `Expected<void>`          | Done                                     |
| `fail(msg, domain)`                       | Done                                     |
| `ANYHOW_TRY*` propagation macros          | Done                                     |
| `map` / `and_then` / `value_or`           | Done                                     |
| `ScopeGuard`                              | Done                                     |
| `context(msg)` -- wrap with message layer | WIP                                      |
| `with_context(fn)` -- lazy context        | WIP                                      |
| `chain()` -- iterate context layers       | WIP                                      |
| `root_cause()` -- deepest error           | WIP                                      |
| `bail!` / `ensure!` macros                | WIP                                      |
| `Result<T>` type alias                    | WIP                                      |
| Downcasting (`is<E>`, `downcast<E>`)      | No `std::error::Error` equivalent in C++ |

## Acknowledgments

Thanks to [Belmu](https://github.com/BelmuTM) -- the original error-handling design in [Noble Engine](https://github.com/BelmuTM/Noble-Engine) is what this started from :)

## License

MIT. See [LICENSE](LICENSE).

<div align="center">
  Made with ❤️
</div>
