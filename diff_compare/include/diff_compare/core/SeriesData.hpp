#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesData {
public:
    SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values);

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    const std::vector<std::string>& values() const noexcept { return values_; }

    std::size_t size() const noexcept { return values_.size(); }

    const std::string& valueAt(std::size_t index) const;

private:
    SeriesDescriptor descriptor_;
    std::vector<std::string> values_;
};

}  // namespace diff_compare
