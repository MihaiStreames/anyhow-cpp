# `anyhow` vs `anyhow-cpp`

A feature-by-feature comparison between [`anyhow`](https://github.com/dtolnay/anyhow) and this port. The goal is API familiarity: if you know `anyhow`, the C++ side should read the same way wherever the language allows it (or where it cannot).

## Table of contents

- [Result type](#result-type)
- [Error construction](#error-construction)
- [Early return](#early-return)
- [Propagation](#propagation)
- [Context](#context)
- [Downcasting](#downcasting)
- [Inspection](#inspection)
- [Backtraces](#backtraces)

## Result type

Rust returns `Result<T, anyhow::Error>`, usually via the `anyhow::Result<T>` alias. C++ has no sum type in the language, so the result is a class wrapping `std::variant<T, Failure>`, with a dedicated `Expected<void>` specialization for functions that yield nothing on success.

```rust
fn load(path: &str) -> anyhow::Result<Config> {
    // ...
}
```

```cpp
anyhow::Expected<Config> load(std::string_view path) {
    // ...
}
```

`anyhow::Result<T>` is available as an alias for `Expected<T>`:

```cpp
anyhow::Result<Config> load(std::string_view path) {
    // ...
}
```

## Error construction

Rust constructs an error from any type implementing `std::error::Error`, or from a string via the `anyhow!` macro. C++ has no error trait, so `fail()` takes a message and an optional `domain` tag that fills the role Rust gives to error types.

```rust
return Err(anyhow!("empty input"));
```

```cpp
return anyhow::fail("empty input", "parse");
```

## Early return

Rust's `bail!` and `ensure!` macros construct an error and return early in one step. C++ mirrors them with `ANYHOW_BAIL` and `ANYHOW_ENSURE`.

```rust
bail!("negative value");
ensure!(!s.is_empty(), "empty input");
```

```cpp
ANYHOW_BAIL("negative value");
ANYHOW_ENSURE(!s.empty(), "empty input");
```

Both accept an optional domain tag as a second argument in C++ (no direct Rust equivalent since Rust uses typed errors for that):

```cpp
ANYHOW_ENSURE(!s.empty(), "empty input", "parse");
```

## Propagation

Rust's `?` operator unwraps on success or returns the error early, converting it through `From`. C++ has no equivalent operator; propagation goes through statement macros that expand to the unwrap-or-return pattern. Each macro captures the current `std::source_location` and pushes it onto the failure's frame buffer, so the trace builds as the error travels up.

```rust
let n = parse(s)?;
validate(n)?;
```

```cpp
int n = 0;
ANYHOW_TRY_ASSIGN(n, parse(s));
ANYHOW_TRY(validate(n));
```

> [!NOTE]
> `ANYHOW_TRY_CATCH(expr, cleanup)` has no Rust analogue -- it runs a cleanup expression before returning on failure. Define `ANYHOW_SHORT_MACROS` before including `macros.hpp` to enable unprefixed aliases for all macros: `TRY`, `TRY_ASSIGN`, `TRY_CATCH`, `BAIL`, `ENSURE`.

## Context

The closest parallel. Both wrap a failure with a human-readable layer as it propagates; `with_context` defers message construction until failure actually occurs.

```rust
parse_file(path)
    .context("failed to parse config")
    .with_context(|| format!("loading config from {path}"))?
```

```cpp
parse_file(path)
    .context("failed to parse config")
    .with_context([&] { return "loading config from " + std::string(path); })
```

Both render context outermost-first, then the root error.

## Downcasting

Rust erases the concrete error type behind `anyhow::Error` but lets you recover it with `downcast_ref::<E>()` / `is::<E>()`.

```rust
if let Some(io_err) = err.downcast_ref::<std::io::Error>() {
    // ...
}
```

C++ has no `std::error::Error` trait, so the approach differs: attach a typed payload via `fail_with()` and recover it with `Failure::downcast<T>()`, which returns a pointer or null.

```cpp
return anyhow::fail_with(IoError {errno}, "read failed", "io");

if (auto* io = failure.downcast<IoError>()) {
    // ...
}
```

The `domain` string tag remains available for cheaper string-based discrimination when a typed payload is not needed.

## Inspection

Rust walks the error's source chain via `Error::chain()` and reaches the deepest cause with `root_cause()`.

```rust
for cause in err.chain() {
    eprintln!("{cause}");
}
let root = err.root_cause();
```

C++ mirrors both on `Failure`. `chain()` returns context layers outermost-first followed by the root error message. `root_cause()` returns the root `ErrorInfo` directly.

```cpp
for (auto cause : failure.chain()) {
    std::cerr << cause << '\n';
}
const auto& root = failure.root_cause();
```

## Backtraces

Rust captures a `std::backtrace::Backtrace` on error creation, gated behind the `RUST_BACKTRACE` environment variable and a nightly/stable feature boundary.

C++ has no environment gate -- frames are captured unconditionally at each propagation point via `std::source_location`, into a fixed-size buffer. Depth defaults to 16 and is overridable at compile time with `ANYHOW_MAX_FRAMES`; when the buffer fills, the oldest frames are evicted.

```rust
// RUST_BACKTRACE=1 at runtime
```

```cpp
#define ANYHOW_MAX_FRAMES 32
#include <anyhow.hpp>
```
