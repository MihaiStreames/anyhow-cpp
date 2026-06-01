#pragma once

/// anyhow-cpp: header-only C++20 result type with propagating stacktraced errors.
///
/// Return `Expected<T>` from fallible functions; `anyhow::fail(msg, domain)` on
/// failure. Propagate with the `ANYHOW_TRY*` macros, which push the current call
/// site onto a frame trace. See the README for usage and examples.

#include "expected.hpp"
#include "failure.hpp"
#include "macros.hpp"
#include "scope_guard.hpp"
