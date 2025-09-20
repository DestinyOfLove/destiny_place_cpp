#include "diff_compare/core/SeriesData.hpp"

#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

SeriesData::SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)), values_(std::move(values)) {}

const std::string& SeriesData::valueAt(std::size_t index) const {
    if (index >= values_.size()) {
        throw std::out_of_range(fmt::format("SeriesData index out of range"));
    }
    return values_[index];
}

}  // namespace diff_compare
