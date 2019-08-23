#include <efi/table/table2d.h>
#include <efi/table/table_bin_array.h>

#include <gtest/gtest.h>

class TestTable2dRatios : public ::testing::Test
{
private:
    std::shared_ptr<TableBins<2>> m_bins;

protected:
    void SetUp() override
    {
        // This test maps [0.20, 0.30] -> [ 0.8, 0.4]
        // Bins are stored at a 100x ratio (1 lsb = 1/100 "real value")
        // Values are stored at a 25x ratio (1 lsb = 1/25 "real value")
        // Both values are stored as integers that represent fractions

        // These values are 100 * [0.2, 0.3]
        static const int bins[] = { 20, 30 };
        m_bins = std::make_shared<TableBinsArray<2, int, std::ratio<100>>>(bins);

        // these values are 25 * [ 0.8, 0.4 ]
        static const int values[] = { 20, 10 };
        dut = std::make_shared<Table2d<2, int, std::ratio<25>>>(*m_bins, values);
    }

    std::shared_ptr<Function<float, float>> dut;
};

TEST_F(TestTable2dRatios, OffScaleLow)
{
    EXPECT_FLOAT_EQ((*dut)(0.1f), 0.8f);
}

TEST_F(TestTable2dRatios, OffScaleHigh)
{
    EXPECT_FLOAT_EQ((*dut)(0.4f), 0.4f);
}

TEST_F(TestTable2dRatios, EdgeLeft)
{
    EXPECT_FLOAT_EQ((*dut)(0.2f), 0.8f);
}

TEST_F(TestTable2dRatios, EdgeRight)
{
    EXPECT_FLOAT_EQ((*dut)(0.3f), 0.4f);
}

TEST_F(TestTable2dRatios, Middle)
{
    // 67% of the way across the interval
    // (0.4-0.8) * 0.67 + 0.8 = 0.532
    EXPECT_FLOAT_EQ((*dut)(0.267f), 0.532f);
}