#pragma once

#define ANYHOW_TRY(expr) \
    do { \
        auto _r_ = (expr); \
        if (!_r_) { \
            return ::anyhow::Unexpected { \
                std::move(_r_.failure()).push(::anyhow::Frame::current()) \
            }; \
        } \
    } while (0)

#define ANYHOW_TRY_CATCH(expr, cleanup) \
    do { \
        auto _r_ = (expr); \
        if (!_r_) { \
            (cleanup); \
            return ::anyhow::Unexpected { \
                std::move(_r_.failure()).push(::anyhow::Frame::current()) \
            }; \
        } \
    } while (0)

#define ANYHOW_TRY_ASSIGN(varOut, expr) \
    do { \
        auto _r_ = (expr); \
        if (!_r_) { \
            return ::anyhow::Unexpected { \
                std::move(_r_.failure()).push(::anyhow::Frame::current()) \
            }; \
        } \
        (varOut) = std::move(_r_.value()); \
    } while (0)

// Opt-in short aliases. Define ANYHOW_SHORT_MACROS before including to enable.
// FAIL omitted -- GTest defines FAIL() and conflicts.
#ifdef ANYHOW_SHORT_MACROS
    #define TRY(expr) ANYHOW_TRY(expr)
    #define TRY_CATCH(expr, c) ANYHOW_TRY_CATCH(expr, c)
    #define TRY_ASSIGN(out, expr) ANYHOW_TRY_ASSIGN(out, expr)
#endif
