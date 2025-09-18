#include "diff_compare/ColumnDescriptor.hpp"

#include <utility>

namespace diff_compare {

ColumnDescriptor::ColumnDescriptor(std::string name, ColumnType type)
    : name_(std::move(name)), type_(type) {}

}  // namespace diff_compare
