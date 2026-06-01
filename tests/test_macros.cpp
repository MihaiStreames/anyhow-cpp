#include <gtest/gtest.h>

#include "helpers.hpp"

static anyhow::Expected<int> inner() {
    return anyhow::fail("inner error");
}

static anyhow::Expected<int> try_propagate() {
    ANYHOW_TRY(inner());
    return anyhow::Expected<int>(0);
}

static anyhow::Expected<int> try_assign_ok() {
    int val = 0;
    ANYHOW_TRY_ASSIGN(val, ok_int(42));
    return anyhow::Expected<int>(val);
}

static anyhow::Expected<int> try_assign_fail() {
    int val = 0;
    ANYHOW_TRY_ASSIGN(val, err_int("assign failed"));
    return anyhow::Expected<int>(val);
}

static anyhow::Expected<int> try_catch_fail(bool& cleaned) {
    ANYHOW_TRY_CATCH(inner(), cleaned = true);
    return anyhow::Expected<int>(0);
}

static anyhow::Expected<int> try_catch_ok(bool& cleaned) {
    ANYHOW_TRY_CATCH(ok_int(1), cleaned = true);
    return anyhow::Expected<int>(0);
}

TEST(Macros, TryPropagatesFailure) {
    auto res = try_propagate();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().message(), "inner error");
}

TEST(Macros, TryAssignExtractsValue) {
    auto res = try_assign_ok();

    ASSERT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 42);
}

TEST(Macros, TryAssignPropagatesFailure) {
    auto res = try_assign_fail();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().message(), "assign failed");
}

TEST(Macros, TryCatchRunsCleanupOnFailure) {
    bool cleaned = false;
    auto res = try_catch_fail(cleaned);

    ASSERT_TRUE(res.failed());
    EXPECT_TRUE(cleaned);
}

TEST(Macros, TryCatchSkipsCleanupOnSuccess) {
    bool cleaned = false;
    auto res = try_catch_ok(cleaned);

    EXPECT_FALSE(res.failed());
    EXPECT_FALSE(cleaned);
}
