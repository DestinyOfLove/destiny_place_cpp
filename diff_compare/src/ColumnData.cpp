#include "diff_compare/ColumnData.hpp"

#include <stdexcept>
#include <utility>

namespace diff_compare {

ColumnData::ColumnData(ColumnDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)), values_(std::move(values)) {}

const std::string& ColumnData::valueAt(std::size_t index) const {
    if (index >= values_.size()) {
        throw std::out_of_range("ColumnData index out of range");
    }
    return values_[index];
}

}  // namespace diff_compare
