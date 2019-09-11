#include <efi/sensor/redundant_sensor.h>

#include <gtest/gtest.h>

#include "mock/mock_sensor.h"

class TestRedundantSensor : public ::testing::Test
{
protected:
	RedundantSensor dut;
	MockSensor m1, m2;

	TestRedundantSensor()
		: dut(SensorType::Tps1, SensorType::Tps1Primary, SensorType::Tps1Secondary, 5.0f, false)
		, m1(SensorType::Tps1Primary)
		, m2(SensorType::Tps1Secondary)
	{
	}

	void SetUp() override
	{
		Sensor::ResetRegistry();

		// Other tests verify registry function - don't re-test it here
		dut.Register();
		m1.Register();
		m2.Register();
	}

	void TearDown() override
	{
		Sensor::ResetRegistry();
	}
};

TEST_F(TestRedundantSensor, SetOnlyOneSensor)
{
	// Don't set any sensors - expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}

	// Set one sensor
	m1.Set(24.0f);

	// Should still be invalid - only one is set!
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}
}

TEST_F(TestRedundantSensor, SetTwoSensors)
{
	// Don't set any sensors - expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}

	// Set one sensor
	m1.Set(24.0f);
	// Set the other sensor
	m2.Set(26.0f);

	// Should now be valid - and the average of the two input
	{
		auto result = dut.Get();
		EXPECT_TRUE(result.Valid);
		EXPECT_FLOAT_EQ(result.Value, 25.0f);
	}
}

TEST_F(TestRedundantSensor, DifferenceNone)
{
	// Set both sensors to the same value
	m1.Set(10);
	m2.Set(10);

	// Expect valid, and 10 output
	{
		auto result = dut.Get();
		EXPECT_TRUE(result.Valid);
		EXPECT_FLOAT_EQ(result.Value, 10.0f);
	}
}

TEST_F(TestRedundantSensor, DifferenceNearLimit)
{
	// Set both sensors to nearly the limit (4.998 apart)
	m1.Set(7.501);
	m2.Set(12.499);

	// Expect valid, and 10 output
	{
		auto result = dut.Get();
		EXPECT_TRUE(result.Valid);
		EXPECT_FLOAT_EQ(result.Value, 10.0f);
	}
}

TEST_F(TestRedundantSensor, DifferenceOverLimit)
{
	// Set both sensors barely over the limit (5.002 apart)
	m1.Set(7.499);
	m2.Set(12.501);

	// Expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}
}

TEST_F(TestRedundantSensor, DifferenceOverLimitSwapped)
{
	// Now try it the other way (m1 > m2)
	m1.Set(12.501);
	m2.Set(7.499);

	// Expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}
}


class TestRedundantSensorIgnoreSecond : public ::testing::Test
{
protected:
	RedundantSensor dut;
	MockSensor m1, m2;

	TestRedundantSensorIgnoreSecond()
		: dut(SensorType::Tps1, SensorType::Tps1Primary, SensorType::Tps1Secondary, 5.0f, true)
		, m1(SensorType::Tps1Primary)
		, m2(SensorType::Tps1Secondary)
	{
	}

	void SetUp() override
	{
		Sensor::ResetRegistry();

		// Other tests verify registry function - don't re-test it here
		dut.Register();
		m1.Register();
		m2.Register();
	}

	void TearDown() override
	{
		Sensor::ResetRegistry();
	}
};

TEST_F(TestRedundantSensorIgnoreSecond, OnlyFirst)
{
	// Don't set any sensors - expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}

	// Set one sensor
	m1.Set(44.0f);

	// Should be valid - we don't care about second sensor
	{
		auto result = dut.Get();
		EXPECT_TRUE(result.Valid);
		EXPECT_FLOAT_EQ(result.Value, 44.0f);
	}
}

TEST_F(TestRedundantSensorIgnoreSecond, OnlySecond)
{
	// Don't set any sensors - expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}

	// Set second sensor only
	m2.Set(66.0f);

	// Should be invalid - should ignore second sensor
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}
}

TEST_F(TestRedundantSensorIgnoreSecond, SetBothIgnoreSecond)
{
	// Don't set any sensors - expect invalid
	{
		auto result = dut.Get();
		EXPECT_FALSE(result.Valid);
	}

	// Set both sensors
	m1.Set(74.0f);
	m2.Set(76.0f);

	// Should be valid, but only get the value from m1
	{
		auto result = dut.Get();
		EXPECT_TRUE(result.Valid);
		EXPECT_FLOAT_EQ(result.Value, 74.0f);
	}
}
