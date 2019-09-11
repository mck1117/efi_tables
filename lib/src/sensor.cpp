#include <efi/sensor/sensor.h>

// This struct represents one sensor in the registry.
// It stores whether the sensor should use a mock value,
// the value to use, and if not a pointer to the sensor that
// can provide a real value.
struct SensorRegistryEntry
{
    bool useMock;
    float mockValue;
    Sensor* sensor;
};

static SensorRegistryEntry s_sensorRegistry[static_cast<size_t>(SensorType::PlaceholderLast)] = {};

bool Sensor::Register()
{
    // Get a ref to where we should be
    auto& entry = s_sensorRegistry[GetIndex()];

    // If there's somebody already here - a consumer tried to double-register a sensor
    if (entry.sensor)
    {
        // This sensor has already been registered. Don't re-register it.
        return false;
    }
    else
    {
        // put ourselves in the registry
        s_sensorRegistry[GetIndex()].sensor = this;
        return true;
    }
}

/*static*/ void Sensor::ResetRegistry()
{
    constexpr size_t len = sizeof(s_sensorRegistry) / sizeof(s_sensorRegistry[0]);

    // Clear all entries
    for (size_t i = 0; i < len; i++)
    {
        auto& entry = s_sensorRegistry[i];

        entry.sensor = nullptr;
        entry.mockValue = 0.0f;
    }
}

/*static*/ SensorRegistryEntry* Sensor::GetEntryForType(SensorType type)
{
    size_t index = GetIndex(type);
    // Check that we didn't get garbage
    if (index >= GetIndex(SensorType::PlaceholderLast))
    {
        return nullptr;
    }

    return &s_sensorRegistry[index];
}

/*static*/ const Sensor* Sensor::GetSensorOfType(SensorType type)
{
    auto entry = GetEntryForType(type);
    return entry ? entry->sensor : nullptr;
}

/*static*/ SensorResult Sensor::Get(SensorType type)
{
    const auto entry = GetEntryForType(type);

    // Check if this is a valid sensor entry
    if (!entry)
    {
        // TODO: warning here
        return { false, 0.0f };
    }

    // Next check for mock
    if (entry->useMock)
    {
        return { true, entry->mockValue };
    }

    // Get the sensor out of the entry
    const Sensor* s = entry->sensor;
    if (s)
    {
        // If we found the sensor, ask it for a result.
        return s->Get();
    }

    // We've exhausted all valid ways to return something - sensor not found.
    return { false, 0 };
}

/*static*/ void Sensor::SetMockValue(SensorType type, float value)
{
    auto entry = GetEntryForType(type);

    if (entry)
    {
        entry->mockValue = value;
        entry->useMock = true;
    }
}

/*static*/ void Sensor::ResetMockValue(SensorType type)
{
    auto entry = GetEntryForType(type);

    if (entry)
    {
        entry->useMock = false;
    }
}

/*static*/ void Sensor::ResetAllMocks()
{
    constexpr size_t len = sizeof(s_sensorRegistry) / sizeof(s_sensorRegistry[0]);

    // Reset all mocks
    for (size_t i = 0; i < len; i++)
    {
        auto& entry = s_sensorRegistry[i];

        entry.useMock = false;
    }
}
