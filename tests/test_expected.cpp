#include <gtest/gtest.h>

#include "helpers.hpp"

TEST(Expected, SuccessHoldsValue) {
    auto res = ok_int(42);

    EXPECT_FALSE(res.failed());
    EXPECT_TRUE(bool(res));
    EXPECT_EQ(res.value(), 42);
    EXPECT_EQ(*res, 42);
}

TEST(Expected, FailureHoldsError) {
    auto res = err_int("something went wrong", "io");

    EXPECT_TRUE(res.failed());
    EXPECT_FALSE(bool(res));
    EXPECT_EQ(res.failure().message(), "something went wrong");
    EXPECT_EQ(res.failure().domain(), "io");
}

TEST(Expected, ArrowOperator) {
    struct S {
        int val = 7;
    };

    anyhow::Expected<S> res(S {});
    EXPECT_EQ(res->val, 7);
}

TEST(Expected, MapSuccess) {
    auto res = std::move(ok_int(3)).map([](int val) { return val * 2; });

    EXPECT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 6);
}

TEST(Expected, MapFailurePassthrough) {
    auto res = std::move(err_int("bad")).map([](int val) { return val * 2; });

    EXPECT_TRUE(res.failed());
    EXPECT_EQ(res.failure().message(), "bad");
}

TEST(Expected, AndThenSuccess) {
    auto res = std::move(ok_int(3)).and_then([](int val) -> anyhow::Expected<int> {
        return anyhow::Expected<int>(val + 1);
    });

    EXPECT_FALSE(res.failed());
    EXPECT_EQ(res.value(), 4);
}

TEST(Expected, AndThenFailurePassthrough) {
    auto res = std::move(err_int("bad")).and_then([](int val) -> anyhow::Expected<int> {
        return anyhow::Expected<int>(val + 1);
    });

    EXPECT_TRUE(res.failed());
    EXPECT_EQ(res.failure().message(), "bad");
}

TEST(Expected, ValueOrSuccess) {
    EXPECT_EQ(std::move(ok_int(5)).value_or(0), 5);
}

TEST(Expected, ValueOrFailure) {
    EXPECT_EQ(std::move(err_int("bad")).value_or(99), 99);
}

TEST(ExpectedVoid, SuccessNotFailed) {
    auto res = ok_void();

    EXPECT_FALSE(res.failed());
    EXPECT_TRUE(bool(res));
}

TEST(ExpectedVoid, FailureHoldsError) {
    auto res = err_void("oops");

    EXPECT_TRUE(res.failed());
    EXPECT_EQ(res.failure().message(), "oops");
}

TEST(ExpectedVoid, AndThenSuccess) {
    bool called = false;
    auto res = std::move(ok_void()).and_then([&]() -> anyhow::Expected<void> {
        called = true;
        return {};
    });

    EXPECT_FALSE(res.failed());
    EXPECT_TRUE(called);
}

TEST(ExpectedVoid, AndThenFailurePassthrough) {
    bool called = false;
    auto res = std::move(err_void("bad")).and_then([&]() -> anyhow::Expected<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(res.failed());
    EXPECT_FALSE(called);
}
