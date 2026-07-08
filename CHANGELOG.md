# Changelog

All notable changes to this project.

## [0.2.0] - 2026-07-08

### Added

- `ANYHOW_ENSURE` stringification (embedding failing condition as a string in the error message)

## [0.1.0] - 2026-06-16

### Added

- `Expected<T>` / `Result<T>` result type with `Unexpected` for explicit error construction
- `Failure` with ring-buffer frame trace, configurable via `ANYHOW_MAX_FRAMES`
- `fail(msg, domain)` and `fail_with(payload, msg, domain)` for typed payloads
- `Failure::downcast<T>()`, `Failure::is<T>()`, `Failure::chain()`, `Failure::root_cause()`
- `Expected<T>::context(msg)` / `with_context(fn)` for wrapping errors with human-readable layers
- `ANYHOW_TRY`, `ANYHOW_TRY_ASSIGN`, `ANYHOW_TRY_CATCH` for error propagation with call-site capture
- `ANYHOW_BAIL`, `ANYHOW_ENSURE` for early return
- `ANYHOW_SHORT_MACROS` opt-in aliases: `TRY`, `TRY_ASSIGN`, `TRY_CATCH`, `BAIL`, `ENSURE`
- `ScopeGuard` with move support and `release()`
- CMake `anyhow::anyhow` target via `FetchContent` or `find_package`
