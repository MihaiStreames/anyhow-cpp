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

/// Early-return a failure.
#define ANYHOW_BAIL(msg, ...) return ::anyhow::fail(msg __VA_OPT__(, ) __VA_ARGS__)

#define ANYHOW_ENSURE_IMPL_MSG(cond, ...)                                                          \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            ANYHOW_BAIL(__VA_ARGS__);                                                              \
        }                                                                                          \
    } while (0)

#define ANYHOW_ENSURE_IMPL_(cond) ANYHOW_ENSURE_IMPL_MSG(cond, "Condition failed: " #cond)

// i couldn't come up with a better solution than this
// VA_OPT(MSG) token-pastes into ANYHOW_ENSURE_IMPL_MSG when message given
// otherwise ANYHOW_ENSURE_IMPL_

/// Early-return a failure if `cond` is false.
#define ANYHOW_ENSURE(cond, ...)                                                                   \
    ANYHOW_ENSURE_IMPL_##__VA_OPT__(MSG)(cond __VA_OPT__(, ) __VA_ARGS__)

// opt-in short aliases
// define ANYHOW_SHORT_MACROS before including to enable

#ifdef ANYHOW_SHORT_MACROS

    #define TRY(expr)                 ANYHOW_TRY(expr)
    #define TRY_CATCH(expr, cleanup)  ANYHOW_TRY_CATCH(expr, cleanup)
    #define TRY_ASSIGN(var_out, expr) ANYHOW_TRY_ASSIGN(var_out, expr)
    #define BAIL(msg, ...)            ANYHOW_BAIL(msg __VA_OPT__(, ) __VA_ARGS__)
    #define ENSURE(cond, ...)         ANYHOW_ENSURE(cond __VA_OPT__(, ) __VA_ARGS__)

#endif
