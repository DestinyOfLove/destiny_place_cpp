#pragma once

#include <string>

#include "diff_compare/ColumnType.hpp"

namespace diff_compare {

class ColumnDescriptor {
public:
    ColumnDescriptor(std::string name, ColumnType type);

    const std::string& name() const noexcept { return name_; }

    ColumnType type() const noexcept { return type_; }

    bool operator==(const ColumnDescriptor& other) const noexcept;

    bool operator!=(const ColumnDescriptor& other) const noexcept { return !(*this == other); }

private:
    std::string name_;
    ColumnType type_;
};

}  // namespace diff_compare
