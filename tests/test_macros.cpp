#include <gtest/gtest.h>

#include "helpers.hpp"

static anyhow::Expected<int> inner() {
    return anyhow::fail("inner error");
}

static anyhow::Expected<int> try_propagate() {
    ANYHOW_TRY(inner());
    return {0};
}

static anyhow::Expected<int> try_assign_ok() {
    int val = 0;
    ANYHOW_TRY_ASSIGN(val, ok_int(42));
    return {val};
}

static anyhow::Expected<int> try_assign_fail() {
    int val = 0;
    ANYHOW_TRY_ASSIGN(val, err_int("assign failed"));
    return {val};
}

static anyhow::Expected<int> try_catch_fail(bool& cleaned) {
    ANYHOW_TRY_CATCH(inner(), cleaned = true);
    return {0};
}

static anyhow::Expected<int> try_catch_ok(bool& cleaned) {
    ANYHOW_TRY_CATCH(ok_int(1), cleaned = true);
    return {0};
}

static anyhow::Expected<int> bail_always() {
    ANYHOW_BAIL("bail triggered");
    return {0};
}

static anyhow::Expected<int> bail_with_domain() {
    ANYHOW_BAIL("oops", "io");
    return {0};
}

static anyhow::Expected<int> ensure_passes(int val) {
    ANYHOW_ENSURE(val > 0, "must be positive");
    return {val};
}

static anyhow::Expected<int> ensure_fails(int val) {
    ANYHOW_ENSURE(val > 0, "must be positive", "validation");
    return {val};
}

static anyhow::Expected<int> ensure_no_msg(int val) {
    ANYHOW_ENSURE(val > 0);
    return {val};
}

TEST(Macros, TryPropagatesFailure) {
    auto res = try_propagate();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.message, "inner error");
}

TEST(Macros, TryAssignExtractsValue) {
    auto res = try_assign_ok();

    ASSERT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 42);
}

TEST(Macros, TryAssignPropagatesFailure) {
    auto res = try_assign_fail();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.message, "assign failed");
}

TEST(Macros, TryCatchRunsCleanupOnFailure) {
    bool cleaned = false;
    auto res     = try_catch_fail(cleaned);

    ASSERT_TRUE(res.failed());
    EXPECT_TRUE(cleaned);
}

TEST(Macros, TryCatchSkipsCleanupOnSuccess) {
    bool cleaned = false;
    auto res     = try_catch_ok(cleaned);

    EXPECT_FALSE(res.failed());
    EXPECT_FALSE(cleaned);
}

TEST(Macros, BailReturnsFailure) {
    auto res = bail_always();
    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.message, "bail triggered");
}

TEST(Macros, BailForwardsDomain) {
    auto res = bail_with_domain();
    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.domain, "io");
}

TEST(Macros, EnsurePassesWhenCondTrue) {
    auto res = ensure_passes(1);
    ASSERT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 1);
}

TEST(Macros, EnsureFailsWhenCondFalse) {
    auto res = ensure_fails(-1);
    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.message, "must be positive");
    EXPECT_EQ(res.failure().error.domain, "validation");
}

TEST(Macros, EnsureAutoMessage) {
    auto res = ensure_no_msg(-1);
    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().error.message, "Condition failed: val > 0");
    EXPECT_EQ(res.failure().error.domain, "");
}
