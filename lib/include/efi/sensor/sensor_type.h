#pragma once

#include <cstdint>

enum class SensorType : uint8_t
{
    Invalid = 0,
    Clt,
    Iat,
    MapBank1,
    MapBank2,
    LambdaBank1,
    LambdaBank2,

    // This is the "resolved" position, potentially composited out of the following two
    Tps1,
    // This is the first sensor
    Tps1Primary,
    // This is the second sensor
    Tps1Secondary,

    Tps2,
    Tps2Primary,
    Tps2Secondary,

    // Leave me at the end!
    PlaceholderLast
};
