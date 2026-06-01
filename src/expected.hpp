#pragma once

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "failure.hpp"

namespace anyhow {

/// Result type for fallible functions: holds either a `T` or a `Failure`.
///
/// Construct from a value on success, or from `anyhow::fail(...)` / `Unexpected`
/// on failure. Check with `failed()` or `operator bool` before accessing `value()`.
template<typename T>
class [[nodiscard]] Expected {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_void_v<T>);

  public:
    explicit Expected(const T& value) : _data(value) {}

    explicit Expected(T&& value) noexcept : _data(std::move(value)) {}

    Expected(Unexpected ux) : _data(std::move(ux.failure)) {}

    explicit operator bool() const noexcept {
        return !failed();
    }

    [[nodiscard]] T& operator*() noexcept {
        return value();
    }

    [[nodiscard]] const T& operator*() const noexcept {
        return value();
    }

    [[nodiscard]] T* operator->() noexcept {
        return &value();
    }

    [[nodiscard]] const T* operator->() const noexcept {
        return &value();
    }

    [[nodiscard]] bool failed() const noexcept {
        return !std::holds_alternative<T>(_data);
    }

    [[nodiscard]] T& value() noexcept {
        assert(!failed());
        return std::get<T>(_data);
    }

    [[nodiscard]] const T& value() const noexcept {
        assert(!failed());
        return std::get<T>(_data);
    }

    [[nodiscard]] Failure& failure() noexcept {
        assert(failed());
        return std::get<Failure>(_data);
    }

    [[nodiscard]] const Failure& failure() const noexcept {
        assert(failed());
        return std::get<Failure>(_data);
    }

    /// Transform the contained value, or pass the failure through untouched.
    template<typename Fn>
    auto map(Fn&& func) && -> Expected<std::invoke_result_t<Fn, T>> {
        using U = std::invoke_result_t<Fn, T>;
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return Expected<U> {std::forward<Fn>(func)(std::move(value()))};
    }

    /// Chain a fallible operation; the function itself returns an `Expected`.
    template<typename Fn>
    auto and_then(Fn&& func) && -> std::invoke_result_t<Fn, T> {
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return std::forward<Fn>(func)(std::move(value()));
    }

    /// Return the contained value, or `fallback` if this holds a failure.
    [[nodiscard]] T value_or(T fallback) && {
        if (failed()) {
            return std::move(fallback);
        }
        return std::move(value());
    }

    /// Wrap the failure with a context message; no-op on success.
    Expected<T> context(std::string msg) && {
        if (failed()) {
            return Unexpected {std::move(failure()).push_context(std::move(msg))};
        }
        return std::move(*this);
    }

    /// Wrap the failure with a lazily-constructed context message; no-op on success.
    template<typename Fn>
    Expected<T> with_context(Fn&& func) && {
        if (failed()) {
            return Unexpected {std::move(failure()).push_context(std::forward<Fn>(func)())};
        }
        return std::move(*this);
    }

  private:
    std::variant<T, Failure> _data;
};

/// Specialization for fallible functions that yield no value on success.
template<>
class [[nodiscard]] Expected<void> {
  public:
    Expected() = default;

    Expected(Unexpected ux) : _failure(std::move(ux.failure)) {}

    explicit operator bool() const noexcept {
        return !failed();
    }

    [[nodiscard]] bool failed() const noexcept {
        return _failure.has_value();
    }

    [[nodiscard]] Failure& failure() {
        assert(_failure.has_value());
        return *_failure;
    }

    [[nodiscard]] const Failure& failure() const {
        assert(_failure.has_value());
        return *_failure;
    }

    /// Chain a fallible operation; the function itself returns an `Expected`.
    template<typename Fn>
    auto and_then(Fn&& func) && -> std::invoke_result_t<Fn> {
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return std::forward<Fn>(func)();
    }

    /// Wrap the failure with a context message; no-op on success.
    Expected<void> context(std::string msg) && {
        if (failed()) {
            return Unexpected {std::move(failure()).push_context(std::move(msg))};
        }
        return {};
    }

    /// Wrap the failure with a lazily-constructed context message; no-op on success.
    template<typename Fn>
    Expected<void> with_context(Fn&& func) && {
        if (failed()) {
            return Unexpected {std::move(failure()).push_context(std::forward<Fn>(func)())};
        }
        return {};
    }

  private:
    std::optional<Failure> _failure;
};

} // namespace anyhow
