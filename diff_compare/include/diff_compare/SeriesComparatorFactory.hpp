#pragma once

#include <memory>

#include "diff_compare/SeriesComparator.hpp"
#include "diff_compare/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesComparatorFactory {
public:
    virtual ~SeriesComparatorFactory() = default;
    virtual std::unique_ptr<SeriesComparator> create(const SeriesDescriptor& descriptor) const;
};

}  // namespace diff_compare
