#include <efi/sensor/stored_value_sensor.h>
#include <efi/function.h>

class ConvertedSensor : public StoredValueSensor
{
public:
    void PostRawValue(float inputValue)
    {
        auto r = ConvertFromInputValue(inputValue);

        if (r.Valid)
        {
            SetValidValue(r.Value);
        }
        else
        {
            Invalidate();
        }
    }

protected:
    ConvertedSensor(SensorType type)
        : StoredValueSensor(type)
    {
    }

    virtual SensorResult ConvertFromInputValue(float inputValue) = 0;
};

class FunctionConvertedSensor final : public ConvertedSensor
{
public:
    FunctionConvertedSensor(SensorType type, Function<SensorResult, float>& converter) : ConvertedSensor(type), m_converter(converter) {}

protected:
    SensorResult ConvertFromInputValue(float inputValue)
    {
        return m_converter(inputValue);
    }

private:
    Function<SensorResult, float>& m_converter;
};
