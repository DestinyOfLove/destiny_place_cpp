#include "diff_compare/SeriesComparatorFactory.hpp"

#include <memory>
#include <stdexcept>

#include "diff_compare/ValueType.hpp"

namespace diff_compare {

std::unique_ptr<SeriesComparator> SeriesComparatorFactory::create(const SeriesDescriptor& descriptor) const {
    switch (descriptor.type()) {
        case ValueType::Integer:
            return std::unique_ptr<SeriesComparator>(new NumericSeriesComparator());
        case ValueType::String:
            return std::unique_ptr<SeriesComparator>(new TextSeriesComparator());
        default:
            break;
    }
    throw std::invalid_argument("Unsupported value type: " + toString(descriptor.type()));
}

}  // namespace diff_compare
