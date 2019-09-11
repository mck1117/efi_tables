#pragma once

#include <efi/sensor/sensor_type.h>

struct SensorResult
{
    const bool Valid;
    const float Value;
};

class Sensor
{
public:
    // Register this sensor in the sensor registry.
    // Returns true if registration succeeded, or false if
    // another sensor of the same type is already registered.
    bool Register();

    // Remove all sensors from the sensor registry - tread carefully if you use this outside of a unit test
    static void ResetRegistry();

    /*
     * Static helper for sensor lookup
     */
    static const Sensor* GetSensorOfType(SensorType type);

    /*
     * Get a reading from the specified sensor.
     */
    static SensorResult Get(SensorType type);

protected:
    Sensor(SensorType type)
        : m_type(type)
    {
    }

private:
    // Retrieve the current reading from the sensor.
    //
    // Override this in a particular sensor's implementation.  As reading sensors is in many hot paths,
    // It is unwise to synchronously read the sensor or do anything otherwise costly here.  At the most,
    // this should be field lookup and simple math.
    virtual SensorResult Get() const = 0;

    SensorType m_type;

    // Get this sensor's index in the list
    constexpr size_t GetIndex()
    {
        return GetIndex(m_type);
    }

    // Get the index in the list for a sensor of particular type
    static constexpr size_t GetIndex(SensorType type)
    {
        return static_cast<size_t>(type);
    }
};
