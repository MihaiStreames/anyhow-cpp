#pragma once

#include <array>
#include <cstdint>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

#include "error.hpp"
#include "frame.hpp"

#ifndef ANYHOW_MAX_FRAMES
    #define ANYHOW_MAX_FRAMES 16
#endif

namespace anyhow {

struct [[nodiscard]] Failure {
    static constexpr std::size_t MAX_FRAMES = ANYHOW_MAX_FRAMES;

    Error error;
    std::array<Frame, MAX_FRAMES> frames {};
    uint8_t count = 0;

    Failure() = default;

    Failure(Error err, Frame frame) : error(std::move(err)) {
        frames[count++] = frame;
    }

    Failure(const Failure&) = default;
    Failure& operator=(const Failure&) = default;
    Failure(Failure&&) = default;
    Failure& operator=(Failure&&) = default;

    Failure&& push(Frame frame) && {
        if (count < MAX_FRAMES) {
            frames[count++] = frame;
        } else {
            for (std::size_t i = 1; i < MAX_FRAMES; ++i) {
                frames[i - 1] = frames[i];
            }
            frames[MAX_FRAMES - 1] = frame;
        }
        return std::move(*this);
    }

    [[nodiscard]] std::string_view message() const noexcept {
        return error.message;
    }

    [[nodiscard]] std::string_view domain() const noexcept {
        return error.domain;
    }
};

struct [[nodiscard]] Unexpected {
    Failure failure;

    explicit Unexpected(Failure fail) : failure(std::move(fail)) {}
};

[[nodiscard]] inline Unexpected fail(
    std::string message,
    std::string domain = {},
    const std::source_location loc = std::source_location::current()
) {
    return Unexpected {Failure {
        Error {.message = std::move(message), .domain = std::move(domain)},
        Frame::current(loc)
    }};
}

} // namespace anyhow
