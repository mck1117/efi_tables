#include <efi/sensor/redundant_sensor.h>

static float FABS(float x) { return x > 0 ? x : -x; }

RedundantSensor::RedundantSensor(SensorType outputType, SensorType first, SensorType second, float maximumDifference, bool ignoreSecondSensor)
	: Sensor(outputType), m_first(first), m_second(second), m_maxDifference(maximumDifference), m_ignoreSecond(ignoreSecondSensor)
{
}

SensorResult RedundantSensor::Get() const
{
	auto result1 = Sensor::Get(m_first);

	// If we're set to disable redundancy, just pass thru the first sensor
	if (m_ignoreSecond)
	{
		return Sensor::Get(m_first);
	}

	auto result2 = Sensor::Get(m_second);

	// If either result is invalid, return invalid.
	if (!result1.Valid || !result2.Valid)
	{
		return { false, 0 };
	}

	// If both are valid, check that they're near one another
	float delta = FABS(result1.Value - result2.Value);
	if (delta > m_maxDifference)
	{
		return { false, 0 };
	}

	// Both sensors are valid, and their readings are close.
	// Return the average.
	float avg = (result1.Value + result2.Value) / 2;
	return { true, avg };
}
