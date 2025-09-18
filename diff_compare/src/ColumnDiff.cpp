#include "diff_compare/ColumnDiff.hpp"

#include <stdexcept>
#include <utility>

namespace diff_compare {

ColumnDiff::ColumnDiff(ColumnDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)), values_(std::move(values)) {}

const std::string& ColumnDiff::valueAt(std::size_t index) const {
    if (index >= values_.size()) {
        throw std::out_of_range("ColumnDiff index out of range");
    }
    return values_[index];
}

}  // namespace diff_compare
