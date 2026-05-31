# Changelog

All notable changes to anyhow-cpp.

## [0.1.1] - 2026-06-01

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
