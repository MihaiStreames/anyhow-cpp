#pragma once

/// anyhow-cpp: header-only C++20 result type with propagating stacktraced errors.
///
/// Return `Expected<T>` (or `Result<T>`) from fallible functions. Construct errors
/// with `fail(msg, domain)` or `fail_with(payload, msg, domain)` for typed payloads
/// recoverable via `Failure::downcast<T>()`. Propagate with `ANYHOW_TRY*` macros,
/// which capture `std::source_location` at each call site and push it onto the frame
/// trace. Return early unconditionally with `ANYHOW_BAIL` or conditionally with
/// `ANYHOW_ENSURE`. See the README for usage and examples.

#include "expected.hpp"
#include "failure.hpp"
#include "macros.hpp"
#include "scope_guard.hpp"

namespace anyhow {

template<typename T>
using Result = Expected<T>;

} // namespace anyhow
