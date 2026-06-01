#pragma once

#include <string>

namespace anyhow {

/// Root error: a human-readable message and an optional domain tag (e.g. "io", "parse").
struct ErrorInfo {
    std::string message;
    std::string domain;
};

} // namespace anyhow
