#include <gtest/gtest.h>

#include "helpers.hpp"

static anyhow::Expected<int> ctx_f() {
    return anyhow::fail("no such file or directory", "io");
}

static anyhow::Expected<int> ctx_g() {
    return std::move(ctx_f()).context("failed to load config");
}

static anyhow::Expected<int> ctx_h() {
    return std::move(ctx_g()).context("failed to start server");
}

TEST(Context, NoopOnSuccess) {
    auto res = std::move(ok_int(1)).context("this should not appear");

    EXPECT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 1);
}

TEST(Context, SingleLayer) {
    auto res = ctx_g();

    ASSERT_TRUE(res.failed());

    EXPECT_EQ(res.failure().context.size(), 1U);
    EXPECT_EQ(res.failure().context[0], "failed to load config");
    EXPECT_EQ(res.failure().error.message, "no such file or directory");
}

TEST(Context, TwoLayers) {
    auto res = ctx_h();

    ASSERT_TRUE(res.failed());

    EXPECT_EQ(res.failure().context.size(), 2U);
    // innermost first in storage
    EXPECT_EQ(res.failure().context[0], "failed to load config");
    EXPECT_EQ(res.failure().context[1], "failed to start server");
}

TEST(Context, WithContextLazy) {
    bool called = false;

    auto res = std::move(err_int("root")).with_context([&] {
        called = true;
        return std::string("outer");
    });

    ASSERT_TRUE(res.failed());
    EXPECT_TRUE(called);
    EXPECT_EQ(res.failure().context.back(), "outer");
}

TEST(Context, WithContextNotCalledOnSuccess) {
    bool called = false;

    auto res = std::move(ok_int(1)).with_context([&] {
        called = true;
        return std::string("outer");
    });

    EXPECT_FALSE(res.failed());
    EXPECT_FALSE(called);
}
