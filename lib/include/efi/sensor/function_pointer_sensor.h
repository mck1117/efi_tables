#include <efi/sensor/sensor.h>

class FunctionPointerSensor final : public Sensor
{
public:
    FunctionPointerSensor(SensorType type, float (*func)())
        : Sensor(type)
        , m_func(func)
    {
    }

    SensorResult Get() const final
    {
        float result = m_func();

        return { true, result };
    }

private:
    float(*m_func)();
};
