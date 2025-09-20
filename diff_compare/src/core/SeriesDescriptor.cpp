#include "diff_compare/core/SeriesDescriptor.hpp"

#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

SeriesDescriptor::SeriesDescriptor(std::string name, ValueType type) : name_(std::move(name)), type_(type) {
    const ValueType inferred = valueTypeFromHeader(name_);
    if (inferred != type_) {
        throw std::invalid_argument(fmt::format(
            "Series name/type mismatch: '{}' implies {} but got {}", name_, toString(inferred), toString(type_)));
    }
}

bool SeriesDescriptor::operator==(const SeriesDescriptor& other) const noexcept {
    return name_ == other.name_ && type_ == other.type_;
}

}  // namespace diff_compare
