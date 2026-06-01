# Changelog

All notable changes to this project.

## [0.1.1] - 2026-06-01

### Added

- Docstrings on public types, members, and macros
- `Failure::context` vector for wrapping human-readable messages
- `Failure::push_context()` mutator for appending context messages
- `Failure::fmt()` renders context outermost-first followed by the root error; `operator<<` wraps it
- `Expected<T>::context(msg)` and `Expected<T>::with_context(fn)` for lazy context attachment (both `T` and `void` specializations)
- GTest suite: 31 tests across `Expected<T/void>`, context, fmt, `ScopeGuard`, and `ANYHOW_TRY*` macros

### Changed

- Dropped redundant `[[nodiscard]]` from `map`, `and_then`, `fail` (return type already carries it)
- `Error` renamed to `ErrorInfo`, freeing `Error` for the future type-erased error type (Phase 5)
- Member ordering standardized across all headers: constants -> fields -> constructors -> mutators -> accessors -> methods
- `Expected<T>` value constructors are now implicit, so `return value;` works directly from a fallible function (the error path stays explicit through `Unexpected`)
- Removed `Failure::message()` / `Failure::domain()` accessors; read `failure().error.message` / `failure().error.domain` directly (`Failure` is a transparent struct)

### Fixed

- `ScopeGuard` move-assign (`operator=(ScopeGuard&&)`) deleted to prevent accidental reassignment
- `i++` to `++i` in `Failure::push` ring-buffer loop

## [0.1.0] - 2026-05-31

### Added

- `Expected<T>` result type with `Unexpected` wrapper for explicit error construction
- `Failure` with ring-buffer frame storage, configurable via `ANYHOW_MAX_FRAMES`
- `Frame` capture via `std::source_location`
- `ScopeGuard` with move support and `release()`
- `ANYHOW_TRY`, `ANYHOW_TRY_CATCH`, `ANYHOW_TRY_ASSIGN` macros for error propagation
- Short aliases via `#define ANYHOW_SHORT_MACROS`
- CMake `anyhow::anyhow` target with `find_package` and `FetchContent` support
