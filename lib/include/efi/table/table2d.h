#pragma once

#include <efi/ratio_helper.h>

#include <efi/table/table_bin.h>
#include <efi/function.h>
#include <efi/interpolate.h>

/**
 * @brief Provides a single-input, single-output lookup table.
 *
 * Computes a lookup table of the form f(x), that is a single numeric output
 * that maps on to a single numeric output.
 */
template <size_t TSize, typename TValue = float, class TRatio = std::ratio<1>>
class Table2d final : public Function<float, float>
{
    // Enforce numeric only (int, float, uintx_t, etc)
    static_assert(std::is_arithmetic_v<TValue>, "Table values must be an arithmetic type");

public:
    Table2d() = delete;

    Table2d(const TableBins<TSize>& bins, const TValue values[TSize])
        : m_bins(&bins)
        , m_values(values)
    {
    }

    float get(float x) const override
    {
        BinResult b = m_bins->Get(x);

        // Get the values to the left and right of the lookup value
        float low = static_cast<float>(m_values[b.Idx]);
        float high = static_cast<float>(m_values[b.Idx + 1]);

        // Interpolate between them as determined by the bin lookup
        float tableValue = linear_interpolate(low, high, b.Frac);

        // Correct by the ratio of table units to "world" units
        return tableValue / GetValueRatio<TRatio>();
    };

private:
    const TableBins<TSize>* const m_bins;
    const TValue* const m_values;
};