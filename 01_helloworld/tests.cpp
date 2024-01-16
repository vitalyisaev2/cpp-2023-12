#include <gtest/gtest.h>

#include "version.h"

TEST(HelloWorld, NonZeroBuildNumber)
{
    ASSERT_GT(BUILD_NUMBER, 0);
}
