#pragma once

#include <string>

#include "diff_compare/core/SeriesData.hpp"

namespace diff_compare {

class SeriesInputProvider {
public:
    virtual ~SeriesInputProvider() = default;
    virtual SeriesData readSeries(const std::string& location) const = 0;
};

}  // namespace diff_compare
