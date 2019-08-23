#pragma once

#include <ratio>

template <class TRatio>
constexpr float GetValueRatio()
{
    return static_cast<float>(TRatio::num) / static_cast<float>(TRatio::den);
}
