#include "diff_compare/ColumnType.hpp"

#include <stdexcept>
#include <string>

namespace diff_compare {

namespace {
const std::string kIntPrefix("Int_");
const std::string kStrPrefix("Str_");
}  // namespace

ColumnType columnTypeFromHeader(const std::string& header) {
    if (header.compare(0, kIntPrefix.size(), kIntPrefix) == 0) {
        return ColumnType::Integer;
    }
    if (header.compare(0, kStrPrefix.size(), kStrPrefix) == 0) {
        return ColumnType::String;
    }
    throw std::invalid_argument("Unsupported column header prefix: " + header);
}

std::string toString(ColumnType type) {
    switch (type) {
        case ColumnType::Integer:
            return "Integer";
        case ColumnType::String:
            return "String";
    }
    throw std::logic_error("Unknown ColumnType");
}

}  // namespace diff_compare
