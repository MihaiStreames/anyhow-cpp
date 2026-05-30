#pragma once

#include <string>

namespace anyhow {

struct Error {
    std::string message;
    std::string domain;
};

} // namespace anyhow
