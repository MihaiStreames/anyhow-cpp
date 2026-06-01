#pragma once

#include <type_traits>
#include <utility>

namespace anyhow {

/// Runs a callable on scope exit unless `release()` is called first.
template<typename Fn>
struct ScopeGuard {
    explicit ScopeGuard(Fn func) : _fn(std::move(func)) {}

    ScopeGuard(ScopeGuard&& other) noexcept : _fn(std::move(other._fn)), _active(other._active) {
        other._active = false;
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    ~ScopeGuard() noexcept {
        if (_active) {
            _fn();
        }
    }

    /// Cancel the guard so the callable does not run.
    void release() noexcept {
        _active = false;
    }

  private:
    std::decay_t<Fn> _fn;
    bool _active = true;
};

template<typename Fn>
ScopeGuard(Fn) -> ScopeGuard<Fn>;

} // namespace anyhow
