#pragma once

#include "diff_compare/SeriesData.hpp"
#include "diff_compare/SeriesDiff.hpp"

namespace diff_compare {

class SeriesComparator {
public:
    virtual ~SeriesComparator() = default;
    virtual SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const = 0;
};

class NumericSeriesComparator : public SeriesComparator {
public:
    SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const override;
};

class TextSeriesComparator : public SeriesComparator {
public:
    SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const override;
};

}  // namespace diff_compare
