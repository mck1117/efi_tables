#pragma once

#include <efi/sensor/sensor.h>



typedef float temperature_t;
typedef int pressure_t;




template <SensorType TSensorType>
struct SensorTypeLookup
{
    using type = float;
};

#define SET_SENSOR_TYPE(sensor, typeName) template<> struct SensorTypeLookup<SensorType::sensor> { using type = typeName; }

SET_SENSOR_TYPE(Clt, temperature_t);
SET_SENSOR_TYPE(Iat, temperature_t);




template <SensorType TSensorType>
class SensorReader final
{
public:
    using SensorValueType = typename SensorTypeLookup<TSensorType>::type;

    SensorReader(SensorValueType defaultValue) : m_defaultValue(defaultValue) { }

    SensorResult Get() const
    {
        return Sensor::Get(TSensorType);
    }

    // Get the sensor's reading, or a default value.
    // Inteded for applications where a default may be used silently,
    // while elsewhere in the world the failed sensor is otherwise handled.
    SensorValueType GetOrDefault() const
    {
        auto result = Get();

        if (result.Valid)
        {
            return (SensorValueType)result.Value;
        }
        else
        {
            return m_defaultValue;
        }
    }
private:
    const SensorValueType m_defaultValue;
};
