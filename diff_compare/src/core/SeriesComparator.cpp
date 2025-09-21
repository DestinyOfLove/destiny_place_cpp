#include "diff_compare/core/SeriesComparator.hpp"

#include <fmt/core.h>

#include <algorithm>
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

long long parseInteger(SeriesData::ViewType value) {
    if (value.empty()) {
        throw std::invalid_argument("Invalid integer value: ''");
    }

    std::size_t index = 0;
    bool negative = false;
    if (value[0] == '-' || value[0] == '+') {
        negative = value[0] == '-';
        ++index;
        if (index == value.size()) {
            throw std::invalid_argument(
                fmt::format("Invalid integer value: '{}'", std::string(value.data(), value.size())));
        }
    }

    long long parsed = 0;
    for (; index < value.size(); ++index) {
        const char ch = value[index];
        if (ch < '0' || ch > '9') {
            throw std::invalid_argument(
                fmt::format("Invalid integer value: '{}'", std::string(value.data(), value.size())));
        }
        parsed = parsed * 10 + (ch - '0');
    }

    return negative ? -parsed : parsed;
}

}  // namespace

SeriesDiff NumericSeriesComparator::compare(const SeriesData& lhs, const SeriesData& rhs) const {
    ensureDescriptorsMatch(lhs, rhs);
    if (lhs.descriptor().type() != ValueType::Integer) {
        throw std::invalid_argument(
            fmt::format("NumericSeriesComparator received {} input", toString(lhs.descriptor().type())));
    }

    if (lhs.hasIntegers() && rhs.hasIntegers()) {
        const auto& lhsInts = lhs.integers();
        const auto& rhsInts = rhs.integers();
        if (lhsInts.size() != rhsInts.size()) {
            throw std::invalid_argument(fmt::format(
                "Series sizes do not match while comparing integers ({} vs {})", lhsInts.size(), rhsInts.size()));
        }

        std::vector<std::string> diffs(lhsInts.size());
        for (std::size_t i = 0; i < lhsInts.size(); ++i) {
            diffs[i] = std::to_string(lhsInts[i] - rhsInts[i]);
        }

        SeriesDescriptor descriptor("Int_Diff", ValueType::Integer);
        return SeriesDiff(std::move(descriptor), std::move(diffs));
    }

    const auto& lhsViews = lhs.views();
    const auto& rhsViews = rhs.views();
    if (lhsViews.size() != rhsViews.size()) {
        throw std::invalid_argument(fmt::format(
            "Series sizes do not match while comparing integers ({} vs {})", lhsViews.size(), rhsViews.size()));
    }

    std::vector<std::string> diffs;
    if (!lhsViews.empty()) {
        diffs.reserve(lhsViews.size());
    }

    for (std::size_t i = 0; i < lhsViews.size(); ++i) {
        const long long left = parseInteger(lhsViews[i]);
        const long long right = parseInteger(rhsViews[i]);
        diffs.emplace_back(std::to_string(left - right));
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

    const auto& lhsViews = lhs.views();
    const auto& rhsViews = rhs.views();
    if (lhsViews.size() != rhsViews.size()) {
        throw std::invalid_argument(fmt::format(
            "Series sizes do not match while comparing strings ({} vs {})", lhsViews.size(), rhsViews.size()));
    }

    std::vector<std::string> diffs;
    if (!lhsViews.empty()) {
        diffs.reserve(lhsViews.size());
    }

    for (std::size_t i = 0; i < lhsViews.size(); ++i) {
        diffs.emplace_back(lhsViews[i] == rhsViews[i] ? "T" : "N");
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

    if (lhs.hasIntegers() && rhs.hasIntegers()) {
        const auto& lhsInts = lhs.integers();
        const auto& rhsInts = rhs.integers();
        if (lhsInts.size() != rhsInts.size()) {
            throw std::invalid_argument(
                fmt::format("Series sizes do not match: {} vs {}", lhsInts.size(), rhsInts.size()));
        }

        const std::size_t count = lhsInts.size();
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
            workers.emplace_back([start, end, &lhsInts, &rhsInts, &diffs]() {
                for (std::size_t i = start; i < end; ++i) {
                    diffs[i] = std::to_string(lhsInts[i] - rhsInts[i]);
                }
            });
        }

        for (std::thread& worker : workers) {
            worker.join();
        }

        SeriesDescriptor descriptor("Int_Diff", ValueType::Integer);
        return SeriesDiff(std::move(descriptor), std::move(diffs));
    }

    const auto& lhsValues = lhs.views();
    const auto& rhsValues = rhs.views();
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

    const auto& lhsValues = lhs.views();
    const auto& rhsValues = rhs.views();
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
