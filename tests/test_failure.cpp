#include <gtest/gtest.h>

#include "helpers.hpp"

struct IoError {
    int code;
};

static anyhow::Expected<int> fail_typed() {
    return anyhow::fail_with(IoError {42}, "io failure", "io");
}

static anyhow::Expected<int> fail_plain() {
    return anyhow::fail("plain failure");
}

TEST(Failure, DowncastHitsCorrectType) {
    auto res = fail_typed();
    ASSERT_TRUE(res.failed());

    const auto* io = res.failure().downcast<IoError>();
    ASSERT_NE(io, nullptr);
    EXPECT_EQ(io->code, 42);
}

TEST(Failure, DowncastWrongTypeReturnsNull) {
    auto res = fail_typed();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().downcast<int>(), nullptr);
}

TEST(Failure, DowncastNoPayloadReturnsNull) {
    auto res = fail_plain();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().downcast<IoError>(), nullptr);
}

TEST(Failure, DowncastMutableAllowsWrite) {
    auto res = fail_typed();
    ASSERT_TRUE(res.failed());

    auto* io = res.failure().downcast<IoError>();
    ASSERT_NE(io, nullptr);

    io->code = 99;
    EXPECT_EQ(res.failure().downcast<IoError>()->code, 99);
}

TEST(Failure, PushFillsBuffer) {
    anyhow::Failure failure;
    failure.error.message = "root";

    for (std::size_t i = 0; i < anyhow::Failure::MAX_FRAMES; ++i) {
        failure = std::move(failure).push(anyhow::Frame::current());
    }

    EXPECT_EQ(failure.count, anyhow::Failure::MAX_FRAMES);
}

TEST(Failure, PushEvictsOldestWhenFull) {
    anyhow::Failure failure;
    failure.error.message = "root";

    for (std::size_t i = 0; i <= anyhow::Failure::MAX_FRAMES; ++i) {
        failure = std::move(failure).push(anyhow::Frame::current());
    }

    EXPECT_EQ(failure.count, anyhow::Failure::MAX_FRAMES);
}

TEST(Failure, IsMatchesPayloadType) {
    auto res = fail_typed();

    ASSERT_TRUE(res.failed());
    EXPECT_TRUE(res.failure().is<IoError>());
    EXPECT_FALSE(res.failure().is<int>());
}

TEST(Failure, IsReturnsFalseWithNoPayload) {
    auto res = fail_plain();

    ASSERT_TRUE(res.failed());
    EXPECT_FALSE(res.failure().is<IoError>());
}

TEST(Failure, RootCauseReturnsError) {
    auto res = fail_plain();

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().root_cause().message, "plain failure");
}

TEST(Failure, ChainNoContextYieldsRootOnly) {
    auto res = fail_plain();
    ASSERT_TRUE(res.failed());

    auto links = res.failure().chain();
    ASSERT_EQ(links.size(), 1U);
    EXPECT_EQ(links[0], "plain failure");
}

TEST(Failure, ChainSizeIncludesAllLayers) {
    auto res = std::move(err_int("root")).context("mid").context("outer");

    ASSERT_TRUE(res.failed());
    EXPECT_EQ(res.failure().chain().size(), 3U);
}

TEST(Failure, ChainReversedIsRootFirst) {
    auto res = std::move(err_int("root")).context("mid").context("outer");
    ASSERT_TRUE(res.failed());

    auto links = res.failure().chain();
    ASSERT_EQ(links.size(), 3U);
    EXPECT_EQ(links.front(), "outer");
    EXPECT_EQ(links.back(), "root");
    EXPECT_EQ(*std::next(links.rbegin()), "mid");
}

TEST(Failure, ChainWithContextOutermostFirst) {
    auto res = std::move(err_int("root error")).context("middle").context("outer");
    ASSERT_TRUE(res.failed());

    auto links = res.failure().chain();
    ASSERT_EQ(links.size(), 3U);
    EXPECT_EQ(links[0], "outer");
    EXPECT_EQ(links[1], "middle");
    EXPECT_EQ(links[2], "root error");
}
