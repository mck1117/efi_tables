#pragma once

#include <cstdint>

struct BinResult 
{
    size_t Idx;
    float Frac;
};

template <size_t TSize>
struct TableBins
{
    // Enforce large enough
    static_assert(TSize > 1);

    virtual BinResult Get(float value) const = 0;
    virtual bool IsAscending() const = 0;
};
