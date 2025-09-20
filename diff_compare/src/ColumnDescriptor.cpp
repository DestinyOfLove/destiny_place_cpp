#include "diff_compare/ColumnDescriptor.hpp"

#include <stdexcept>
#include <utility>

namespace diff_compare {

ColumnDescriptor::ColumnDescriptor(std::string name, ColumnType type) : name_(std::move(name)), type_(type) {
    const ColumnType inferred = columnTypeFromHeader(name_);
    if (inferred != type_) {
        throw std::invalid_argument("Column name/type mismatch: " + name_ + " implies " + toString(inferred)
                                    + " but got " + toString(type_));
    }
}

bool ColumnDescriptor::operator==(const ColumnDescriptor& other) const noexcept {
    return name_ == other.name_ && type_ == other.type_;
}

}  // namespace diff_compare
