# `anyhow` vs `anyhow-cpp`

A feature-by-feature comparison between [`anyhow`](https://github.com/dtolnay/anyhow) and this port. The goal is API familiarity: if you know `anyhow`, the C++ side should read the same way wherever the language allows it (or where it cannot).

## Table of contents

- [Result type](#result-type)
- [Error construction](#error-construction)
- [Propagation](#propagation)
- [Context](#context)
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

> [!NOTE]
> A `Result<T>` alias matching Rust's naming is planned but not yet shipped.

## Error construction

Rust constructs an error from any type implementing `std::error::Error`, or from a string via the `anyhow!` macro. C++ has no error trait, so `fail()` takes a message and an optional `domain` tag that fills the role Rust gives to error types.

```rust
return Err(anyhow!("empty input"));
```

```cpp
return anyhow::fail("empty input", "parse");
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
> `ANYHOW_TRY_CATCH(expr, cleanup)` has no Rust analogue -- it runs a cleanup expression before returning on failure. Define `ANYHOW_SHORT_MACROS` for the unprefixed `TRY` / `TRY_ASSIGN` / `TRY_CATCH` aliases.

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
