#pragma once

#include <functional>
#include <memory>

#include "diff_compare/SeriesComparator.hpp"
#include "diff_compare/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesComparatorFactory {
public:
    virtual ~SeriesComparatorFactory() = default;
    using ComparatorBuilder = std::function<std::unique_ptr<SeriesComparator>()>;
    virtual std::unique_ptr<SeriesComparator> create(const SeriesDescriptor& descriptor) const;
    static void registerComparator(ValueType type, ComparatorBuilder builder);
};

}  // namespace diff_compare
