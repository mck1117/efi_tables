
#pragma once

#include <efi/table_bin.h>
#include <efi/interpolate.h>

#include <efi/ratio_helper.h>


template<size_t TSize, class TBin = float, class TRatio = std::ratio<1>>
class TableBinsArray final : public TableBins<TSize>
{
    // Enforce arithmetic only (int, float, uintx_t, etc)
    static_assert(std::is_arithmetic_v<TBin>, "Table bins must be an arithmetic type");

    const TBin* const m_bins;

public:
    // no default - require an array
    TableBinsArray() = delete;

    TableBinsArray(const TBin(&bins)[TSize]) : m_bins(bins) { }

    /**
     * @brief Finds the location of a value in the bin array.
     *
     * @param value The value to find in the bins.
     * @return A result containing the index to the left of the value,
     * and how far from (idx) to (idx + 1) the value is located.
     */
    BinResult Get(float value) const override
    {
        // Adjust the "real" value to the "table" values
        value *= GetValueRatio<TRatio>();

        // Handle off-scale low
        if (value <= m_bins[0])
        {
            return { 0, 0.0f };
        }

        // Handle off-scale high
        if (value >= m_bins[TSize - 1])
        {
            return { TSize - 2, 1.0f };
        }

        decltype(BinResult::Idx) idx = 0;

        // Find the last index less than the searched value
        // Linear search for now, maybe binary search in future
        // after collecting real perf data
        for (idx = 0; idx < TSize - 1; idx++)
        {
            if (m_bins[idx + 1] > value)
            {
                break;
            }
        }

        float low = static_cast<float>(m_bins[idx]);
        float high = static_cast<float>(m_bins[idx + 1]);

        // Compute how far along the bin we are
        // (0.0f = left side, 1.0f = right side)
        float fraction = (value - low) / (high - low);

        return { idx, fraction };
    }

    /**
     * Confirm that the bins are in strictly ascending order.
     *
     * @return True if m_bins[i] <= m_bins[i+1] for all i, otherwise false.
     */
    bool IsAscending() const override
    {
        for (size_t i = 0; i < TSize - 1; i++)
        {
            if (m_bins[i] >= m_bins[i + 1])
            {
                return false;
            }
        }

        return true;
    }
};
