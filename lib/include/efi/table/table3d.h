
#include <ratio>
#include <efi/ratio_helper.h>
#include <efi/function.h>

#include "table_bin.h"

template <typename TValue, size_t TRows, size_t TColumns, class TRatio = std::ratio<1>>
class Table3d final : public Function<float, float, float>
{
public:
    Table3d(const char* name, const TableBins<TRows>& rowBins, const TableBins<TColumns>& columnBins, const TValue values[TRows * TColumns])
        : m_rowBins(rowBins)
        , m_columnBins(columnBins)
        , m_values(values)
    {
    }

    float get(float rowValue, float columnValue) override const
    {
        BinResult row = m_rowBins.Get(rowValue);
        BinResult col = m_columnBins.Get(columnValue);

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
    const TableBins<TRows>& const m_rowBins;
    const TableBins<TColumns>& const m_columnBins;
    const TValue* const m_values;

    static size_t GetIndexForCoordinates(size_t row, size_t column)
    {
        // TODO: is row or column correct here?
        return row * TColumns + column;
    }

    TValue GetValueAtPosition(size_t row, size_t column)
    {
        return m_values[GetIndexForCoordinates(row, column)];
    }
};