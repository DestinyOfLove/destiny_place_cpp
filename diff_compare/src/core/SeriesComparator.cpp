#include "diff_compare/core/SeriesComparator.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "diff_compare/core/ValueType.hpp"
#include <fmt/core.h>

namespace diff_compare {
namespace {
void ensureComparable(const SeriesData& lhs, const SeriesData& rhs) {
    if (!(lhs.descriptor() == rhs.descriptor())) {
        throw std::invalid_argument(fmt::format("Series descriptors do not match: {}/{} vs {}/{}",
                                                lhs.descriptor().name(),
                                                toString(lhs.descriptor().type()),
                                                rhs.descriptor().name(),
                                                toString(rhs.descriptor().type())));
    }
    if (lhs.size() != rhs.size()) {
        throw std::invalid_argument(fmt::format("Series sizes do not match: {} vs {}", lhs.size(), rhs.size()));
    }
}

long long parseInteger(const std::string& value) {
    std::size_t idx = 0;
    long long parsed = 0;
    try {
        parsed = std::stoll(value, &idx);
    } catch (const std::exception&) {
        throw std::invalid_argument(fmt::format("Invalid integer value: '{}'", value));
    }
    if (idx != value.size()) {
        throw std::invalid_argument(fmt::format("Invalid integer value: '{}'", value));
    }
    return parsed;
}
}  // namespace

SeriesDiff NumericSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureComparable(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::Integer) {
        throw std::invalid_argument(
            fmt::format("NumericSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    std::vector<std::string> diffs;
    diffs.reserve(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        const long long left = parseInteger(lhs.valueAt(i));
        const long long right = parseInteger(rhs.valueAt(i));
        const long long delta = left - right;
        diffs.emplace_back(std::to_string(delta));
    }
    SeriesDescriptor descriptor("Int_Diff", ValueType::Integer);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

SeriesDiff TextSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureComparable(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::String) {
        throw std::invalid_argument(
            fmt::format("TextSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    std::vector<std::string> diffs;
    diffs.reserve(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        diffs.emplace_back(lhs.valueAt(i) == rhs.valueAt(i) ? "T" : "N");
    }
    SeriesDescriptor descriptor("Str_Diff", ValueType::String);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

}  // namespace diff_compare
