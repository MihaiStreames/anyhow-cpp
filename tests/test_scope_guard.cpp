#include <gtest/gtest.h>

#include "scope_guard.hpp"

TEST(ScopeGuard, RunsOnExit) {
    bool ran = false;
    {
        auto guard = anyhow::ScopeGuard([&] { ran = true; });
    }

    EXPECT_TRUE(ran);
}

TEST(ScopeGuard, SkipsAfterRelease) {
    bool ran = false;
    {
        auto guard = anyhow::ScopeGuard([&] { ran = true; });
        guard.release();
    }

    EXPECT_FALSE(ran);
}

TEST(ScopeGuard, MoveTransfersOwnership) {
    bool ran = false;
    {
        auto guard = anyhow::ScopeGuard([&] { ran = true; });
        auto moved = std::move(guard);
    }

    EXPECT_TRUE(ran);
}

TEST(ScopeGuard, MoveSourceDoesNotRun) {
    int count = 0;
    {
        auto guard = anyhow::ScopeGuard([&] { ++count; });
        auto moved = std::move(guard);
    }

    EXPECT_EQ(count, 1);
}
