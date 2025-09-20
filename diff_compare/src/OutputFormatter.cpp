#include "diff_compare/OutputFormatter.hpp"

#include <vector>

namespace diff_compare {

std::vector<std::string> PlainTextOutputFormatter::format(const SeriesDiff& diff) const {
    std::vector<std::string> lines;
    lines.reserve(diff.size() + 1);
    lines.emplace_back(diff.descriptor().name());
    const auto& values = diff.values();
    lines.insert(lines.end(), values.begin(), values.end());
    return lines;
}

}  // namespace diff_compare
