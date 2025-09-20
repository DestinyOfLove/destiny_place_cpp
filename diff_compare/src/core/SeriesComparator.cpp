#include "diff_compare/core/SeriesComparator.hpp"

#include <fmt/core.h>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "diff_compare/core/SeriesData.hpp"
#include "diff_compare/core/ValueType.hpp"

namespace diff_compare {
namespace {

void ensureDescriptorsMatch(const SeriesData& lhs, const SeriesData& rhs) {
    if (!(lhs.descriptor() == rhs.descriptor())) {
        throw std::invalid_argument(fmt::format("Series descriptors do not match: {}/{} vs {}/{}",
                                                lhs.descriptor().name(),
                                                toString(lhs.descriptor().type()),
                                                rhs.descriptor().name(),
                                                toString(rhs.descriptor().type())));
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
    ensureDescriptorsMatch(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::Integer) {
        throw std::invalid_argument(
            fmt::format("NumericSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    std::unique_ptr<SeriesCursor> lhsCursor = lhs.cursor();
    std::unique_ptr<SeriesCursor> rhsCursor = rhs.cursor();
    if (!lhsCursor || !rhsCursor) {
        throw std::invalid_argument("Series cursor creation failed");
    }

    std::vector<std::string> diffs;
    const std::size_t anticipated = lhs.size();
    if (anticipated > 0) {
        diffs.reserve(anticipated);
    }

    std::string lhsValue;
    std::string rhsValue;
    std::size_t processed = 0;
    while (true) {
        const bool lhsHas = lhsCursor->next(lhsValue);
        const bool rhsHas = rhsCursor->next(rhsValue);
        if (!lhsHas || !rhsHas) {
            if (lhsHas != rhsHas) {
                throw std::invalid_argument(
                    fmt::format("Series sizes do not match while comparing integers (processed {} rows)", processed));
            }
            break;
        }
        const long long left = parseInteger(lhsValue);
        const long long right = parseInteger(rhsValue);
        const long long delta = left - right;
        diffs.emplace_back(std::to_string(delta));
        ++processed;
    }

    SeriesDescriptor descriptor("Int_Diff", ValueType::Integer);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

SeriesDiff TextSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureDescriptorsMatch(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::String) {
        throw std::invalid_argument(
            fmt::format("TextSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    std::unique_ptr<SeriesCursor> lhsCursor = lhs.cursor();
    std::unique_ptr<SeriesCursor> rhsCursor = rhs.cursor();
    if (!lhsCursor || !rhsCursor) {
        throw std::invalid_argument("Series cursor creation failed");
    }

    std::vector<std::string> diffs;
    const std::size_t anticipated = lhs.size();
    if (anticipated > 0) {
        diffs.reserve(anticipated);
    }

    std::string lhsValue;
    std::string rhsValue;
    std::size_t processed = 0;
    while (true) {
        const bool lhsHas = lhsCursor->next(lhsValue);
        const bool rhsHas = rhsCursor->next(rhsValue);
        if (!lhsHas || !rhsHas) {
            if (lhsHas != rhsHas) {
                throw std::invalid_argument(
                    fmt::format("Series sizes do not match while comparing strings (processed {} rows)", processed));
            }
            break;
        }
        diffs.emplace_back(lhsValue == rhsValue ? "T" : "N");
        ++processed;
    }

    SeriesDescriptor descriptor("Str_Diff", ValueType::String);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

}  // namespace diff_compare
