#include "diff_compare/ColumnComparatorFactory.hpp"

#include <memory>
#include <stdexcept>

#include "diff_compare/ColumnType.hpp"

namespace diff_compare {

std::unique_ptr<ColumnComparator> ColumnComparatorFactory::create(const ColumnDescriptor& descriptor) const {
    switch (descriptor.type()) {
        case ColumnType::Integer:
            return std::make_unique<IntColumnComparator>();
        case ColumnType::String:
            return std::make_unique<StringColumnComparator>();
        default:
            break;
    }
    throw std::invalid_argument("Unsupported column type: " + toString(descriptor.type()));
}

}  // namespace diff_compare
