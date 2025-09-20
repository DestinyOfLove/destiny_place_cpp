#include "diff_compare/io/ColumnParser.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"

namespace diff_compare {

std::string SimpleColumnParser::sanitizeLine(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    auto is_space = [](unsigned char ch) { return std::isspace(ch) != 0; };

    const auto first = std::find_if_not(line.begin(), line.end(), is_space);
    const auto last = std::find_if_not(line.rbegin(), line.rend(), is_space).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}

SeriesData SimpleColumnParser::parse(std::istream& input) const {
    std::string header;
    if (!std::getline(input, header)) {
        throw std::invalid_argument(fmt::format("Input column missing header"));
    }
    header = sanitizeLine(std::move(header));
    const ValueType type = valueTypeFromHeader(header);

    std::vector<std::string> values;
    std::string value;
    while (std::getline(input, value)) {
        value = sanitizeLine(std::move(value));
        if (value.empty()) {
            continue;  // 忽略空行（包括仅含回车的行）
        }
        values.emplace_back(std::move(value));
    }

    return SeriesData(SeriesDescriptor(header, type), std::move(values));
}

}  // namespace diff_compare
