#pragma once

#include <string>

#include "diff_compare/core/SeriesDiff.hpp"

namespace diff_compare {

class SeriesOutputWriter {
public:
    virtual ~SeriesOutputWriter() = default;
    virtual void writeSeries(const SeriesDiff& diff, const std::string& location) const = 0;
};

}  // namespace diff_compare
