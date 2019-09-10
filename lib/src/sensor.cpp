#include <efi/sensor/sensor.h>

static Sensor* s_sensorRegistry[static_cast<size_t>(SensorType::PlaceholderLast)] = {};

bool Sensor::Register()
{
    // Get a ref to where we should be
    auto& entry = s_sensorRegistry[GetIndex()];

    // If there's somebody already here - a consumer tried to double-register a sensor
    if (entry)
    {
        // uhh, somebody already registered this sensor?
        return false;
    }
    else
    {
        // put ourselves in the registry
        s_sensorRegistry[GetIndex()] = this;
        return true;
    }
}

/*static*/ void Sensor::ResetRegistry()
{
    constexpr size_t len = sizeof(s_sensorRegistry) / sizeof(s_sensorRegistry[0]);

    // Clear all entries
    for(size_t i = 0; i < len; i++)
    {
        s_sensorRegistry[i] = nullptr;
    }
}

/*static*/ const Sensor* Sensor::GetSensorOfType(SensorType type)
{
    size_t index = GetIndex(type);
    // Check that we didn't get garbage
    if (index >= GetIndex(SensorType::PlaceholderLast))
    {
        return nullptr;
    }

    return s_sensorRegistry[index];
}

/*static*/ SensorResult Sensor::Get(SensorType type)
{
    const Sensor* s = GetSensorOfType(type);

    if (s)
    {
        // If we found the sensor, ask it for a result.
        return s->Get();
    }
    else
    {
        // If we didn't find the sensor - indicate that
        return { false, 0 };
    }
}