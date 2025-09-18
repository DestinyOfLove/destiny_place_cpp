#pragma once

#include <string>

namespace diff_compare {

enum class ColumnType {
    Integer,
    String
};

ColumnType columnTypeFromHeader(const std::string& header);
std::string toString(ColumnType type);

}  // namespace diff_compare
