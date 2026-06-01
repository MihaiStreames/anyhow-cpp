#pragma once

#include <type_traits>
#include <utility>

namespace anyhow {

/// Runs a callable on scope exit unless `release()` is called first.
template<typename Fn>
struct ScopeGuard {
    std::decay_t<Fn> fn;
    bool active = true;

    explicit ScopeGuard(Fn func) : fn(std::move(func)) {}

    ScopeGuard(ScopeGuard&& other) noexcept : fn(std::move(other.fn)), active(other.active) {
        other.active = false;
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    ~ScopeGuard() noexcept {
        if (active) {
            fn();
        }
    }

    /// Cancel the guard so the callable does not run.
    void release() noexcept {
        active = false;
    }
};

template<typename Fn>
ScopeGuard(Fn) -> ScopeGuard<Fn>;

} // namespace anyhow
