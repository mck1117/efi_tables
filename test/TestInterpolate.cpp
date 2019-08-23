#include <efi/interpolate.h>

#include <gtest/gtest.h>

TEST(Interpolate, TestLeftEdge)
{
    EXPECT_FLOAT_EQ(23, linear_interpolate(23, 27, 0));
}

TEST(Interpolate, TestCenter)
{
    EXPECT_FLOAT_EQ(25, linear_interpolate(23, 27, 0.5));
}

TEST(Interpolate, TestRightEdge)
{
    EXPECT_FLOAT_EQ(27, linear_interpolate(23, 27, 1));
}

TEST(Interpolate, TestOffRightEdgeWithoutClamp)
{
    EXPECT_FLOAT_EQ(31, linear_interpolate(23, 27, 2));
}

TEST(Interpolate, TestOffLeftEdgeWithoutClamp)
{
    EXPECT_FLOAT_EQ(19, linear_interpolate(23, 27, -1));
}

TEST(Interpolate, TestOffRightEdgeWithClamp)
{
    EXPECT_FLOAT_EQ(27, linear_interpolate_clamped(23, 27, 2));
}

TEST(Interpolate, TestOffLeftEdgeWithClamp)
{
    EXPECT_FLOAT_EQ(23, linear_interpolate_clamped(23, 27, -1));
}
