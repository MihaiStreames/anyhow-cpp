#pragma once

#include "anyhow.hpp"

inline anyhow::Expected<int> ok_int(int val) {
    return anyhow::Expected<int>(val);
}

inline anyhow::Expected<int> err_int(std::string msg, std::string domain = {}) {
    return anyhow::fail(std::move(msg), std::move(domain));
}

inline anyhow::Expected<void> ok_void() {
    return {};
}

inline anyhow::Expected<void> err_void(std::string msg) {
    return anyhow::fail(std::move(msg));
}
