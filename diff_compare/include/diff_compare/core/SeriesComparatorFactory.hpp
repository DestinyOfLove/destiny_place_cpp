#pragma once

#include <memory>

#include "diff_compare/core/SeriesComparator.hpp"
#include "diff_compare/core/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesComparatorFactory {
public:
    std::unique_ptr<SeriesComparator> create(const SeriesDescriptor& descriptor) const;
};

}  // namespace diff_compare
