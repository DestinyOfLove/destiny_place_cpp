#include "diff_compare/core/SeriesDiff.hpp"

#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

SeriesDiff::SeriesDiff(SeriesDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)), values_(std::move(values)) {}

const std::string& SeriesDiff::valueAt(std::size_t index) const {
    if (index >= values_.size()) {
        throw std::out_of_range(fmt::format("SeriesDiff index out of range"));
    }
    return values_[index];
}

}  // namespace diff_compare
