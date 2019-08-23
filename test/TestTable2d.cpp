#include <efi/table2d.h>
#include <efi/table_bin_array.h>

#include <gtest/gtest.h>

class TestTable2dSmall : public ::testing::Test
{
private:
    std::shared_ptr<TableBins<2>> m_bins;

protected:
    void SetUp() override
    {
        // This test maps [20,30] -> [100,200]

        static const float bins[] = { 20, 30 };
        m_bins = std::make_shared<TableBinsArray<2>>(bins);

        static const float values[] = { 100, 200 };
        dut = std::make_shared<Table2d<2>>(*m_bins, values);
    }

    std::shared_ptr<Function<float, float>> dut;
};

TEST_F(TestTable2dSmall, OffScaleLow)
{
    EXPECT_FLOAT_EQ((*dut)(10), 100);
}

TEST_F(TestTable2dSmall, OffScaleHigh)
{
    EXPECT_FLOAT_EQ((*dut)(40), 200);
}

TEST_F(TestTable2dSmall, EdgeLeft)
{
    EXPECT_FLOAT_EQ((*dut)(20), 100);
}

TEST_F(TestTable2dSmall, EdgeRight)
{
    EXPECT_FLOAT_EQ((*dut)(30), 200);
}

TEST_F(TestTable2dSmall, Middle)
{
    EXPECT_FLOAT_EQ((*dut)(25), 150);
}

class Test2dTableMassive : public ::testing::Test
{
    static constexpr int Count = 2500;

private:
    float bins[Count];
    float values[Count];

    std::shared_ptr<TableBins<Count>> m_bins;

protected:
    void SetUp() override
    {
        float x = 0;

        for (size_t i = 0; i < std::size(bins); i++)
        {
            x += 0.1f;
            bins[i] = x;
            values[i] = x * x;
        }

        m_bins = std::make_shared<TableBinsArray<Count>>(bins);
        dut = std::make_shared<Table2d<Count>>(*m_bins, values);
    }

    std::shared_ptr<Function<float, float>> dut;
};

TEST_F(Test2dTableMassive, t)
{
    float x = 0;
    float maxErr = -1;

    for (size_t i = 0; i < 25000; i++)
    {
        x += 0.01f;

        float actual = x * x;
        float lookup = (*dut)(x);

        float err = std::abs(actual - lookup);

        if (err > maxErr)
        {
            maxErr = err;
        }
    }

    EXPECT_LT(maxErr, 0.01);
}