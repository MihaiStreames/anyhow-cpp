#pragma once

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "failure.hpp"

namespace anyhow {

template<typename T>
class [[nodiscard]] Expected {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_void_v<T>);

  public:
    explicit Expected(const T& value) : _data(value) {}

    explicit Expected(T&& value) noexcept : _data(std::move(value)) {}

    Expected(Unexpected ux) : _data(std::move(ux.failure)) {}

    [[nodiscard]] bool failed() const noexcept {
        return !std::holds_alternative<T>(_data);
    }

    explicit operator bool() const noexcept {
        return !failed();
    }

    [[nodiscard]] T& value() noexcept {
        assert(!failed());
        return std::get<T>(_data);
    }

    [[nodiscard]] const T& value() const noexcept {
        assert(!failed());
        return std::get<T>(_data);
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

    [[nodiscard]] Failure& failure() noexcept {
        assert(failed());
        return std::get<Failure>(_data);
    }

    [[nodiscard]] const Failure& failure() const noexcept {
        assert(failed());
        return std::get<Failure>(_data);
    }

    template<typename Fn>
    [[nodiscard]] auto map(Fn&& fn) && -> Expected<std::invoke_result_t<Fn, T>> {
        using U = std::invoke_result_t<Fn, T>;
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return Expected<U> {std::forward<Fn>(fn)(std::move(value()))};
    }

    template<typename Fn>
    [[nodiscard]] auto and_then(Fn&& fn) && -> std::invoke_result_t<Fn, T> {
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return std::forward<Fn>(fn)(std::move(value()));
    }

    [[nodiscard]] T value_or(T fallback) && {
        if (failed()) {
            return std::move(fallback);
        }
        return std::move(value());
    }

  private:
    std::variant<T, Failure> _data;
};

template<>
class [[nodiscard]] Expected<void> {
  public:
    Expected() = default;

    Expected(Unexpected ux) : _failure(std::move(ux.failure)) {}

    [[nodiscard]] bool failed() const noexcept {
        return _failure.has_value();
    }

    explicit operator bool() const noexcept {
        return !failed();
    }

    [[nodiscard]] Failure& failure() {
        assert(_failure.has_value());
        return *_failure;
    }

    [[nodiscard]] const Failure& failure() const {
        assert(_failure.has_value());
        return *_failure;
    }

    template<typename Fn>
    [[nodiscard]] auto and_then(Fn&& fn) && -> std::invoke_result_t<Fn> {
        if (failed()) {
            return Unexpected {std::move(failure())};
        }
        return std::forward<Fn>(fn)();
    }

  private:
    std::optional<Failure> _failure;
};

} // namespace anyhow
