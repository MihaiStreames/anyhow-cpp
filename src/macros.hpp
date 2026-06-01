#pragma once

/// Evaluate `expr`; on failure, push the current frame and return the failure.
#define ANYHOW_TRY(expr)                                                                           \
    do {                                                                                           \
        auto _r_ = (expr);                                                                         \
        if (!_r_) {                                                                                \
            return ::anyhow::Unexpected {                                                          \
                std::move(_r_.failure()).push(::anyhow::Frame::current())                          \
            };                                                                                     \
        }                                                                                          \
    } while (0)

/// Like `ANYHOW_TRY`, but run `cleanup` before returning on failure.
#define ANYHOW_TRY_CATCH(expr, cleanup)                                                            \
    do {                                                                                           \
        auto _r_ = (expr);                                                                         \
        if (!_r_) {                                                                                \
            (cleanup);                                                                             \
            return ::anyhow::Unexpected {                                                          \
                std::move(_r_.failure()).push(::anyhow::Frame::current())                          \
            };                                                                                     \
        }                                                                                          \
    } while (0)

/// Evaluate `expr`; on success assign the value to `var_out`, else return the failure.
#define ANYHOW_TRY_ASSIGN(var_out, expr)                                                           \
    do {                                                                                           \
        auto _r_ = (expr);                                                                         \
        if (!_r_) {                                                                                \
            return ::anyhow::Unexpected {                                                          \
                std::move(_r_.failure()).push(::anyhow::Frame::current())                          \
            };                                                                                     \
        }                                                                                          \
        (var_out) = std::move(_r_.value());                                                        \
    } while (0)

// opt-in short aliases
// define ANYHOW_SHORT_MACROS before including to enable
// FAIL omitted (GTest defines FAIL() and conflicts)

#ifdef ANYHOW_SHORT_MACROS

    #define TRY(expr) ANYHOW_TRY(expr)
    #define TRY_CATCH(expr, cleanup) ANYHOW_TRY_CATCH(expr, cleanup)
    #define TRY_ASSIGN(var_out, expr) ANYHOW_TRY_ASSIGN(var_out, expr)

#endif
