#pragma once

#include <cstdint>
#include <source_location>

namespace anyhow {

/// A single call site in an error's propagation trace.
struct Frame {
    const char* function;
    const char* file;
    uint32_t line;

    /// Capture the caller's location. Defaulted argument resolves at the call site.
    static Frame
    current(const std::source_location loc = std::source_location::current()) noexcept {
        return {.function = loc.function_name(), .file = loc.file_name(), .line = loc.line()};
    }
};

} // namespace anyhow
