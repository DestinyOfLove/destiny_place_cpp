#pragma once

#include <string>

#include "diff_compare/ValueType.hpp"

namespace diff_compare {

class SeriesDescriptor {
public:
    SeriesDescriptor(std::string name, ValueType type);

    const std::string& name() const noexcept { return name_; }

    ValueType type() const noexcept { return type_; }

    bool operator==(const SeriesDescriptor& other) const noexcept;

    bool operator!=(const SeriesDescriptor& other) const noexcept { return !(*this == other); }

private:
    std::string name_;
    ValueType type_;
};

}  // namespace diff_compare
