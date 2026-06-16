#pragma once

#include <any>
#include <string>

namespace anyhow {

/// Root error: a human-readable message, an optional domain tag, and an optional typed payload.
///
/// `payload` is empty by default. Set it via `fail_with()` and recover with `Failure::downcast<T>()`.
struct ErrorInfo {
    std::string message; ///< Human-readable description of what went wrong.
    std::string domain; ///< Optional category tag (e.g. "io", "parse"). Empty if unset.
    std::any    payload; ///< Optional typed value for programmatic inspection. Empty if unset.
};

} // namespace anyhow
