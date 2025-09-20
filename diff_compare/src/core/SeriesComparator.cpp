#include "diff_compare/core/SeriesComparator.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <thread>
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

namespace {

std::size_t effectiveThreads(std::size_t requested, std::size_t work_items) {
    std::size_t hw = std::max<std::size_t>(1, std::thread::hardware_concurrency());
    std::size_t threads = requested == 0 ? hw : requested;
    threads = std::max<std::size_t>(1, threads);
    if (work_items == 0) {
        return 1;
    }
    return std::min(threads, work_items);
}

}  // namespace

SeriesDiff ParallelNumericSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureDescriptorsMatch(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::Integer) {
        throw std::invalid_argument(
            fmt::format("ParallelNumericSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    const std::vector<std::string>& lhsValues = lhs.values();
    const std::vector<std::string>& rhsValues = rhs.values();
    if (lhsValues.size() != rhsValues.size()) {
        throw std::invalid_argument(
            fmt::format("Series sizes do not match: {} vs {}", lhsValues.size(), rhsValues.size()));
    }

    const std::size_t count = lhsValues.size();
    const std::size_t threads = effectiveThreads(thread_count_, count);
    if (threads <= 1 || count == 0) {
        NumericSeriesComparator fallback;
        return fallback.compare(lhs, rhs);
    }

    std::vector<std::string> diffs(count);
    std::vector<std::thread> workers;
    workers.reserve(threads);
    const std::size_t block = (count + threads - 1) / threads;

    for (std::size_t t = 0; t < threads; ++t) {
        const std::size_t start = t * block;
        if (start >= count) {
            break;
        }
        const std::size_t end = std::min(count, start + block);
        workers.emplace_back([start, end, &lhsValues, &rhsValues, &diffs]() {
            for (std::size_t i = start; i < end; ++i) {
                const long long left = parseInteger(lhsValues[i]);
                const long long right = parseInteger(rhsValues[i]);
                diffs[i] = std::to_string(left - right);
            }
        });
    }

    for (std::thread& worker : workers) {
        worker.join();
    }

    SeriesDescriptor descriptor("Int_Diff", ValueType::Integer);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

SeriesDiff ParallelTextSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureDescriptorsMatch(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::String) {
        throw std::invalid_argument(
            fmt::format("ParallelTextSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    const std::vector<std::string>& lhsValues = lhs.values();
    const std::vector<std::string>& rhsValues = rhs.values();
    if (lhsValues.size() != rhsValues.size()) {
        throw std::invalid_argument(
            fmt::format("Series sizes do not match: {} vs {}", lhsValues.size(), rhsValues.size()));
    }

    const std::size_t count = lhsValues.size();
    const std::size_t threads = effectiveThreads(thread_count_, count);
    if (threads <= 1 || count == 0) {
        TextSeriesComparator fallback;
        return fallback.compare(lhs, rhs);
    }

    std::vector<std::string> diffs(count);
    std::vector<std::thread> workers;
    workers.reserve(threads);
    const std::size_t block = (count + threads - 1) / threads;

    for (std::size_t t = 0; t < threads; ++t) {
        const std::size_t start = t * block;
        if (start >= count) {
            break;
        }
        const std::size_t end = std::min(count, start + block);
        workers.emplace_back([start, end, &lhsValues, &rhsValues, &diffs]() {
            for (std::size_t i = start; i < end; ++i) {
                diffs[i] = lhsValues[i] == rhsValues[i] ? "T" : "N";
            }
        });
    }

    for (std::thread& worker : workers) {
        worker.join();
    }

    SeriesDescriptor descriptor("Str_Diff", ValueType::String);
    return SeriesDiff(std::move(descriptor), std::move(diffs));
}

}  // namespace diff_compare
