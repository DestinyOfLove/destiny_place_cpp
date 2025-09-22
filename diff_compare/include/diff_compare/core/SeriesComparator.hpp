#pragma once

#include <cstddef>
#include <memory>

#include "diff_compare/core/SeriesData.hpp"
#include "diff_compare/core/SeriesDiff.hpp"

namespace diff_compare {

class SeriesComparator {
public:
    virtual ~SeriesComparator() = default;
    virtual SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const = 0;
};

class ParallelNumericSeriesComparator : public SeriesComparator {
public:
    explicit ParallelNumericSeriesComparator(std::size_t thread_count = 0) : thread_count_(thread_count) {}

    SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const override;

private:
    std::size_t thread_count_;
};

class ParallelTextSeriesComparator : public SeriesComparator {
public:
    explicit ParallelTextSeriesComparator(std::size_t thread_count = 0) : thread_count_(thread_count) {}

    SeriesDiff compare(const SeriesData& lhs, const SeriesData& rhs) const override;

private:
    std::size_t thread_count_;
};

}  // namespace diff_compare
