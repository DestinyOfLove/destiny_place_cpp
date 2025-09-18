#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "diff_compare/ColumnDescriptor.hpp"

namespace diff_compare {

class ColumnDiff {
public:
    ColumnDiff(ColumnDescriptor descriptor, std::vector<std::string> values);

    const ColumnDescriptor& descriptor() const noexcept { return descriptor_; }
    const std::vector<std::string>& values() const noexcept { return values_; }

    std::size_t size() const noexcept { return values_.size(); }
    const std::string& valueAt(std::size_t index) const;

private:
    ColumnDescriptor descriptor_;
    std::vector<std::string> values_;
};

}  // namespace diff_compare
