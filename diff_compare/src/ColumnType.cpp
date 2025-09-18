#include "diff_compare/ColumnType.hpp"

#include <stdexcept>
#include <string_view>

namespace diff_compare {

namespace {
constexpr std::string_view kIntPrefix{"Int_"};
constexpr std::string_view kStrPrefix{"Str_"};
}  // namespace

ColumnType columnTypeFromHeader(const std::string& header) {
    if (header.rfind(kIntPrefix.data(), 0) == 0) {
        return ColumnType::Integer;
    }
    if (header.rfind(kStrPrefix.data(), 0) == 0) {
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
