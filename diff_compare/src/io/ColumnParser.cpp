#include "diff_compare/io/ColumnParser.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <boost/utility/string_view.hpp>
#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"

namespace diff_compare {

boost::string_view SimpleColumnParser::trimView(const std::string& line) {
    const char* begin = line.data();
    const char* end = begin + line.size();
    while (begin < end && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }
    while (end > begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    return boost::string_view(begin, static_cast<std::size_t>(end - begin));
}

std::string SimpleColumnParser::sanitizeLine(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    const boost::string_view view = trimView(line);
    if (view.empty()) {
        return {};
    }
    return std::string(view.begin(), view.end());
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
        const boost::string_view view = trimView(value);
        if (view.empty()) {
            continue;  // 忽略空行（包括仅含回车的行）
        }
        values.emplace_back(view.begin(), view.end());
    }

    return SeriesData(SeriesDescriptor(header, type), std::move(values));
}

}  // namespace diff_compare
