#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <ranges>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include "error.hpp"
#include "frame.hpp"

#ifndef ANYHOW_MAX_FRAMES
    #define ANYHOW_MAX_FRAMES 16
#endif

namespace anyhow {

/// An error plus the frame trace it accumulated while propagating.
///
/// `frames` is a ring buffer of depth `ANYHOW_MAX_FRAMES` (default 16); once full,
/// the oldest frame is evicted on each `push`.
struct [[nodiscard]] Failure {
    static constexpr std::size_t MAX_FRAMES = ANYHOW_MAX_FRAMES;

    ErrorInfo error;
    std::vector<std::string> context;
    std::array<Frame, MAX_FRAMES> frames {};
    uint8_t count = 0;

    Failure() = default;

    Failure(ErrorInfo err, Frame frame) : error(std::move(err)) {
        frames[count++] = frame;
    }

    Failure(const Failure&) = default;
    Failure& operator=(const Failure&) = default;
    Failure(Failure&&) = default;
    Failure& operator=(Failure&&) = default;

    /// Append a frame to the trace, evicting the oldest if the buffer is full.
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

    /// Push a wrapping context message (outermost pushed last).
    Failure&& push_context(std::string msg) && {
        context.push_back(std::move(msg));
        return std::move(*this);
    }

    /// Render the failure as a human-readable string.
    ///
    /// Context layers are printed outermost-first, followed by the root error.
    [[nodiscard]] std::string fmt() const {
        std::string out;

        for (const auto& layer : std::views::reverse(context)) {
            out += layer;
            out += '\n';
        }

        out += error.message;

        if (!error.domain.empty()) {
            out += " [";
            out += error.domain;
            out += ']';
        }

        return out;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Failure& failure) {
    return os << failure.fmt();
}

/// Wraps a `Failure` for return. Forces explicit error construction, preventing
/// implicit conversion of a bare value into an errored `Expected<T>`.
struct [[nodiscard]] Unexpected {
    Failure failure;

    explicit Unexpected(Failure payload) : failure(std::move(payload)) {}
};

/// Construct a failure with `message` and optional `domain`, capturing the call site.
inline Unexpected fail(
    std::string message,
    std::string domain = {},
    const std::source_location loc = std::source_location::current()
) {
    return Unexpected {Failure {
        ErrorInfo {.message = std::move(message), .domain = std::move(domain)},
        Frame::current(loc)
    }};
}

} // namespace anyhow
