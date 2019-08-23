#include <efi/table_bin_array.h>

#include <memory>

#include <gtest/gtest.h>

static void EXPECT_BINRESULT(const BinResult& actual, size_t expectedIdx, float expectedFrac)
{
    EXPECT_EQ(actual.Idx, expectedIdx);
    EXPECT_NEAR(actual.Frac, expectedFrac, expectedFrac / 1e4);
}

// Test with small bins: only two values
static const float s_smallBins[] = { 10, 20 };

class TableBinsSmall : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_bins = std::make_shared<TableBinsArray<2>>(s_smallBins);
    }

    std::shared_ptr<TableBins<2>> m_bins;
};

TEST_F(TableBinsSmall, OffScaleLeft)
{
    EXPECT_BINRESULT(m_bins->Get(5), 0, 0);
}

TEST_F(TableBinsSmall, OffScaleRight)
{
    EXPECT_BINRESULT(m_bins->Get(25), 0, 1);
}

TEST_F(TableBinsSmall, EdgeLeft)
{
    EXPECT_BINRESULT(m_bins->Get(10), 0, 0);
}

TEST_F(TableBinsSmall, EdgeRight)
{
    EXPECT_BINRESULT(m_bins->Get(10), 0, 0);
}

TEST_F(TableBinsSmall, Middle)
{
    EXPECT_BINRESULT(m_bins->Get(15), 0, 0.5f);
}

TEST_F(TableBinsSmall, IsAscending)
{
    EXPECT_TRUE(m_bins->IsAscending());
}


// Test with medium bins, 3 items
static const float s_bigBins[] = { 10, 20, 30 };

class TableBinsBig : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_bins = std::make_shared<TableBinsArray<3>>(s_bigBins);
    }

    std::shared_ptr<TableBins<3>> m_bins;
};


TEST_F(TableBinsBig, OffScaleLow)
{
    EXPECT_BINRESULT(m_bins->Get(5), 0, 0);
}

TEST_F(TableBinsBig, OffScaleHigh)
{
    EXPECT_BINRESULT(m_bins->Get(35), 1, 1.0f);
}


TEST_F(TableBinsBig, NearMiddleLow)
{
    EXPECT_BINRESULT(m_bins->Get(19.99f), 0, 0.999f);
}

TEST_F(TableBinsBig, NearMiddleExact)
{
    EXPECT_BINRESULT(m_bins->Get(20.0f), 1, 0);
}

TEST_F(TableBinsBig, NearMiddleHigh)
{
    EXPECT_BINRESULT(m_bins->Get(20.01f), 1, 0.001f);
}

TEST_F(TableBinsBig, LeftMiddle)
{
    EXPECT_BINRESULT(m_bins->Get(15.0f), 0, 0.5f);
}

TEST_F(TableBinsBig, RightMiddle)
{
    EXPECT_BINRESULT(m_bins->Get(25.0f), 1, 0.5f);
}

TEST_F(TableBinsBig, IsAscending)
{
    EXPECT_TRUE(m_bins->IsAscending());
}

TEST(TableBinsNotAscending, IsNotAscending)
{
    const float values[] = { 10, 20, 30, 40, 60, 50 };
    TableBinsArray bins(values);

    EXPECT_FALSE(bins.IsAscending());
}