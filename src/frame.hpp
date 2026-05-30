#pragma once

#include <cstdint>
#include <source_location>

namespace anyhow {

struct Frame {
    const char* function;
    const char* file;
    uint32_t    line;

    static Frame
    current(const std::source_location loc = std::source_location::current()) noexcept {
        return {loc.function_name(), loc.file_name(), loc.line()};
    }
};

} // namespace anyhow
