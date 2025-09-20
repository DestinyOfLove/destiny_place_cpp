#pragma once

#include <string>

namespace diff_compare {

enum class ValueType {
    Integer,
    String
};

ValueType valueTypeFromHeader(const std::string& header);
std::string toString(ValueType type);

}  // namespace diff_compare
