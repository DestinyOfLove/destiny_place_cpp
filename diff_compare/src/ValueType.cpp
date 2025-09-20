#include "diff_compare/ValueType.hpp"

#include <stdexcept>
#include <string>

namespace diff_compare {

namespace {
const std::string kIntPrefix("Int_");
const std::string kStrPrefix("Str_");
}  // namespace

ValueType valueTypeFromHeader(const std::string& header) {
    if (header.compare(0, kIntPrefix.size(), kIntPrefix) == 0) {
        return ValueType::Integer;
    }
    if (header.compare(0, kStrPrefix.size(), kStrPrefix) == 0) {
        return ValueType::String;
    }
    throw std::invalid_argument("Unsupported column header prefix: " + header);
}

std::string toString(ValueType type) {
    switch (type) {
        case ValueType::Integer:
            return "Integer";
        case ValueType::String:
            return "String";
    }
    throw std::logic_error("Unknown ValueType");
}

}  // namespace diff_compare
