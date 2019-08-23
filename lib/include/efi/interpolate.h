#pragma once

constexpr float linear_interpolate(float low, float high, float frac)
{
    return high * frac + low * (1 - frac);
}

constexpr float linear_interpolate_clamped(float low, float high, float frac)
{
    if (frac > 1.0f)
    {
        frac = 1.0f;
    }
    else if (frac < 0.0f)
    {
        frac = 0.0f;
    }

    return linear_interpolate(low, high, frac);
}
