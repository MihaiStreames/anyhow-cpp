#pragma once

#include <cstdint>
#include <source_location>

namespace anyhow {

struct Frame {
    const char* function;
    const char* file;
    uint32_t line;

    static Frame
    current(const std::source_location loc = std::source_location::current()) noexcept {
        return {.function = loc.function_name(), .file = loc.file_name(), .line = loc.line()};
    }
};

} // namespace anyhow
