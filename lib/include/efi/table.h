/**
 *  @file Table.h
 *  @brief Table (2d and 3d) look up and interpolation logic
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <ratio>

template <class TRatio>
constexpr float GetValueRatio()
{
    return static_cast<float>(TRatio::num) / static_cast<float>(TRatio::den);
}

struct BinResult
{
    uint8_t Idx;
    float Frac;
};

namespace TableHelper
{
    constexpr float Linterp(float low, float high, float frac)
    {
        return high * frac + low * (1 - frac);
    }
};


template <size_t TSize>
struct TableBins
{
    // Enforce large enough
    static_assert(TSize > 1);

    virtual BinResult Get(float value) const = 0;
    virtual bool IsAscending() const = 0;
};

template<class TBin, size_t TSize, class TRatio = std::ratio<1>>
class TableBinsArray final : public TableBins<TSize>
{
    // Enforce arithmetic only (int, float, uintx_t, etc)
    static_assert(std::is_arithmetic_v<TBin>, "Table bins must be an arithmetic type");

    const TBin* const m_bins;

public:
    // no default - require an array
    TableBinsArray() = delete;

    TableBinsArray(TBin(&bins)[TSize]) : m_bins(bins) { }

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

        uint8_t idx = 0;

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

template <class TReturn, class... TArgs>
struct Function
{
    [[nodiscard]] virtual TReturn operator()(TArgs... args) const = 0;
};

/**
 * @brief Provides a single-input, single-output lookup table.
 *
 * Computes a lookup table of the form f(x), that is a single numeric output
 * that maps on to a single numeric output.
 */
template <typename TValue, size_t TSize, class TRatio = std::ratio<1>>
class Table2d final : public Function<float, float>
{
    // Enforce numeric only (int, float, uintx_t, etc)
    static_assert(std::is_arithmetic_v<TValue>, "Table values must be an arithmetic type");

public:
    Table2d() = delete;

    Table2d(const TableBins<TSize>& bins, TValue values[TSize])
        : m_bins(&bins)
        , m_values(values)
    {
    }

    float operator()(float x) const override
    {
        BinResult b = m_bins->Get(x);

        // Get the values to the left and right of the lookup value
        float low = static_cast<float>(m_values[b.Idx]);
        float high = static_cast<float>(m_values[b.Idx + 1]);

        // Interpolate between them as determined by the bin lookup
        float tableValue = TableHelper::Linterp(low, high, b.Frac);

        // Correct by the ratio of table units to "world" units
        return tableValue / GetValueRatio<TRatio>();
    };

private:
    const TableBins<TSize>* const m_bins;
    const TValue* const m_values;
};

template <typename TValue, size_t TRows, size_t TColumns, class TRatio = std::ratio<1>>
class Table3d final : public Function<float, float>
{
public:
    Table3d(const char* name, const TableBins<TRows>& rowBins, const TableBins<TColumns>& columnBins, TValue values[TRows * TColumns])
        : m_rowBins(&rowBins)
        , m_columnBins(&columnBins)
        , m_values(values)
    {
    }

    float operator()(float rowValue, float columnValue) const
    {
        BinResult row = m_rowBins->Get(rowValue);
        BinResult col = m_columnBins->Get(columnValue);

        // Orient the table such that (0, 0) is the bottom left corner,
        // then the following variable names will make sense
        float lowerLeft = GetValueAtPosition(row.Idx, col.Idx);
        float upperLeft = GetValueAtPosition(row.Idx + 1, col.Idx);
        float lowerRight = GetValueAtPosition(row.Idx, col.Idx + 1);
        float upperRight = GetValueAtPosition(row.Idx + 1, col.Idx + 1);

        // Interpolate each side by itself
        float left = TableHelper::Linterp(lowerLeft, upperLeft, row.Frac);
        float right = TableHelper::Linterp(lowerRight, upperRight, row.Frac);

        // Then interpolate between those
        float tableValue = TableHelper::Linterp(left, right, col.Frac);

        // Correct by the ratio of table units to "world" units
        return tableValue / GetValueRatio<TRatio>();
    }

private:
    const TableBins<TRows>* const m_rowBins;
    const TableBins<TColumns>* const m_columnBins;
    const TValue* const m_values;

    static size_t GetIndexForCoordinates(size_t row, size_t column)
    {
        // TODO: what ordering is correct here?
        return row * TColumns + column;
    }

    TValue GetValueAtPosition(size_t row, size_t column)
    {
        return m_values[GetIndexForCoordinates(row, column)];
    }
};
