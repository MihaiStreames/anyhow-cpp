#pragma once

#include <type_traits>
#include <utility>

namespace anyhow {

template <typename Fn> struct ScopeGuard {
    std::decay_t<Fn> fn;
    bool             active = true;

    explicit ScopeGuard(Fn f) : fn(std::move(f)) {}

    ScopeGuard(ScopeGuard&& other) noexcept : fn(std::move(other.fn)), active(other.active) {
        other.active = false;
    }

    ScopeGuard(const ScopeGuard&)            = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ~ScopeGuard() noexcept {
        if (active) fn();
    }

    void release() noexcept { active = false; }
};

template <typename Fn> ScopeGuard(Fn) -> ScopeGuard<Fn>;

} // namespace anyhow
