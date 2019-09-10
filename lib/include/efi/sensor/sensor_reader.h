#pragma once

#include <efi/sensor/sensor.h>

template <SensorType TSensorType>
class SensorReader final
{
public:
    SensorReader(float defaultValue) : m_defaultValue(defaultValue) { }

    SensorResult Get() const
    {
        return Sensor::Get(TSensorType);
    }

    float GetOrDefault() const
    {
        auto result = Get();

        if (result.Valid)
        {
            return result.Value;
        }
        else
        {
            return m_defaultValue;
        }
    }
private:
    const float m_defaultValue;
};
