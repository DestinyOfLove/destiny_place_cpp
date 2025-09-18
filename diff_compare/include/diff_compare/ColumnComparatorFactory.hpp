#pragma once

#include <memory>

#include "diff_compare/ColumnComparator.hpp"
#include "diff_compare/ColumnDescriptor.hpp"

namespace diff_compare {

class ColumnComparatorFactory {
public:
    virtual ~ColumnComparatorFactory() = default;
    virtual std::unique_ptr<ColumnComparator> create(const ColumnDescriptor& descriptor) const;
};

}  // namespace diff_compare
