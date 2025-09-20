#pragma once

#include <string>

namespace diff_compare {

enum class ValueType {
    Integer,
    String
};

struct ValueTraits {
    ValueType type;
    std::string label;
};

ValueType valueTypeFromHeader(const std::string& header);
std::string toString(ValueType type);
void registerValueType(std::string prefix, ValueTraits traits);

}  // namespace diff_compare
