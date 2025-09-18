#include "diff_compare/ColumnComparator.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "diff_compare/ColumnType.hpp"

namespace diff_compare {
namespace {
void ensureComparable(const ColumnData& lhs, const ColumnData& rhs) {
    if (lhs.descriptor().type() != rhs.descriptor().type()) {
        throw std::invalid_argument("Column types do not match: " +
                                    toString(lhs.descriptor().type()) + " vs " +
                                    toString(rhs.descriptor().type()));
    }
    if (lhs.size() != rhs.size()) {
        throw std::invalid_argument("Column sizes do not match: " + std::to_string(lhs.size()) +
                                    " vs " + std::to_string(rhs.size()));
    }
}

long long parseInteger(const std::string& value) {
    std::size_t idx = 0;
    long long parsed = 0;
    try {
        parsed = std::stoll(value, &idx);
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid integer value: " + value);
    }
    if (idx != value.size()) {
        throw std::invalid_argument("Invalid integer value: " + value);
    }
    return parsed;
}
}

ColumnDiff IntColumnComparator::compare(const ColumnData& lhs, const ColumnData& rhs) const {
    ensureComparable(lhs, rhs);
    if (lhs.descriptor().type() != ColumnType::Integer) {
        throw std::invalid_argument("IntColumnComparator received non-integer input");
    }

    std::vector<std::string> diffs;
    diffs.reserve(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        const long long left = parseInteger(lhs.valueAt(i));
        const long long right = parseInteger(rhs.valueAt(i));
        const long long delta = left - right;
        diffs.emplace_back(std::to_string(delta));
    }
    ColumnDescriptor descriptor("Diff", ColumnType::Integer);
    return ColumnDiff(std::move(descriptor), std::move(diffs));
}

ColumnDiff StringColumnComparator::compare(const ColumnData& lhs, const ColumnData& rhs) const {
    ensureComparable(lhs, rhs);
    if (lhs.descriptor().type() != ColumnType::String) {
        throw std::invalid_argument("StringColumnComparator received non-string input");
    }

    std::vector<std::string> diffs;
    diffs.reserve(lhs.size());
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        diffs.emplace_back(lhs.valueAt(i) == rhs.valueAt(i) ? "T" : "N");
    }
    ColumnDescriptor descriptor("Diff", ColumnType::String);
    return ColumnDiff(std::move(descriptor), std::move(diffs));
}

}  // namespace diff_compare
