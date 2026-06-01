# anyhow-cpp&ensp;¯\\\_(°ペ)\_/¯

Header-only C++20 result type with propagating stacktraced errors, inspired by Rust's [`anyhow`](https://github.com/dtolnay/anyhow).

[![C++](https://img.shields.io/badge/20-f34b7d?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![Release](https://img.shields.io/github/v/release/MihaiStreames/anyhow-cpp?label=release)](https://github.com/MihaiStreames/anyhow-cpp/releases)
[![CI](https://github.com/MihaiStreames/anyhow-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/MihaiStreames/anyhow-cpp/actions/workflows/ci.yml)
[![License](https://img.shields.io/github/license/MihaiStreames/anyhow-cpp?label=license)](LICENSE)

```cpp
#include <anyhow.hpp>
```

## Install

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  anyhow-cpp
  GIT_REPOSITORY https://github.com/MihaiStreames/anyhow-cpp.git
  GIT_TAG        v0.1.1
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

> [!NOTE]
> Define `ANYHOW_SHORT_MACROS` before including `macros.hpp` to enable the short aliases `TRY`, `TRY_ASSIGN`, `TRY_CATCH`.

Use `Expected<T>` as the return type of any fallible function. Return `anyhow::fail(message, domain)` on failure, or return the value directly on success.

```cpp
anyhow::Expected<int> parse(std::string_view s) {
    if (s.empty()) {
        return anyhow::fail("empty input", "parse");
    }

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
    std::cout << "error [" << f.error.domain << "]: " << f.error.message << '\n';

    for (size_t i = 0; i < f.count; i++) {
        std::cout << "  at " << f.frames[i].function << " (" << f.frames[i].file << ':' << f.frames[i].line << ")\n";
    }
}
```

```console
error [parse]: empty input
  at anyhow::Expected<int> parse(std::string_view) (src/main.cpp:8)
  at anyhow::Expected<std::string> process(std::string_view) (src/main.cpp:17)
```

### Context

Wrap failures with human-readable context as they propagate up the call stack. `context` takes a string eagerly; `with_context` takes a callable and only evaluates it on failure.

```cpp
anyhow::Expected<Config> load(std::string_view path) {
    return parse_file(path)
        .context("failed to parse config")
        .with_context([&] { return "loading config from " + std::string(path); });
}
```

On failure, `Failure::fmt()` renders context outermost-first followed by the root error:

```console
loading config from /etc/app/config.toml
failed to parse config
unexpected token at line 42 [parse]
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

## vs `anyhow`

See [docs/vs-rust.md](docs/vs-rust.md) for a feature-by-feature comparison with side-by-side Rust and C++ examples.

## Acknowledgments

Thanks to [Belmu](https://github.com/BelmuTM) -- the original error-handling design in [Noble Engine](https://github.com/BelmuTM/Noble-Engine) is what this started from :)

## License

MIT. See [LICENSE](LICENSE).

<div align="center">
  Made with ❤️
</div>
