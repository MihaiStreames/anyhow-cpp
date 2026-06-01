#include <gtest/gtest.h>

#include <sstream>

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

TEST(Fmt, RootOnly) {
    EXPECT_EQ(err_int("something went wrong").failure().fmt(), "something went wrong");
}

TEST(Fmt, RootWithDomain) {
    EXPECT_EQ(err_int("permission denied", "io").failure().fmt(), "permission denied [io]");
}

TEST(Fmt, ContextOutermostFirst) {
    // outermost context printed first
    const std::string expected =
        "failed to start server\n"
        "failed to load config\n"
        "no such file or directory [io]";
    EXPECT_EQ(ctx_h().failure().fmt(), expected);
}

TEST(Fmt, OperatorStream) {
    std::ostringstream oss;
    oss << ctx_h().failure();

    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str().find("failed to start server"), std::string::npos);
}
