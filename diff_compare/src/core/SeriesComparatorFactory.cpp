#include "diff_compare/core/SeriesComparatorFactory.hpp"

#include <memory>
#include <stdexcept>

#include "diff_compare/core/ValueType.hpp"
#include <fmt/core.h>

namespace diff_compare {

std::unique_ptr<SeriesComparator> SeriesComparatorFactory::create(const SeriesDescriptor& descriptor) const {
    switch (descriptor.type()) {
        case ValueType::Integer:
            return std::unique_ptr<SeriesComparator>(new ParallelNumericSeriesComparator());
        case ValueType::String:
            return std::unique_ptr<SeriesComparator>(new ParallelTextSeriesComparator());
        default:
            throw std::invalid_argument(fmt::format("Unsupported value type: {}", toString(descriptor.type())));
    }
}

}  // namespace diff_compare
