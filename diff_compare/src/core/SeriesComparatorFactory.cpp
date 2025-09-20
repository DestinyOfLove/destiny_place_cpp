#include "diff_compare/core/SeriesComparatorFactory.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

#include "diff_compare/core/ValueType.hpp"
#include <fmt/core.h>

namespace diff_compare {

namespace {

static_assert(static_cast<int>(ValueType::Count) == 2,
              "Update comparator registry initialization when adding new ValueType values.");

std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder>& comparatorRegistry() {
    static std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder> registry = [] {
        std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder> initial;
        initial.emplace(ValueType::Integer,
                        [] { return std::unique_ptr<SeriesComparator>(new ParallelNumericSeriesComparator()); });
        initial.emplace(ValueType::String,
                        [] { return std::unique_ptr<SeriesComparator>(new ParallelTextSeriesComparator()); });
        return initial;
    }();
    return registry;
}

std::mutex& registryMutex() {
    static std::mutex mutex;
    return mutex;
}

}  // namespace

void SeriesComparatorFactory::registerComparator(ValueType type, ComparatorBuilder builder) {
    std::lock_guard<std::mutex> lock(registryMutex());
    if (!builder) {
        throw std::invalid_argument(fmt::format("Comparator builder for {} must be non-null", toString(type)));
    }
    comparatorRegistry()[type] = std::move(builder);
}

std::unique_ptr<SeriesComparator> SeriesComparatorFactory::create(const SeriesDescriptor& descriptor) const {
    std::lock_guard<std::mutex> lock(registryMutex());
    auto& registry = comparatorRegistry();
    const auto it = registry.find(descriptor.type());
    if (it == registry.end()) {
        throw std::invalid_argument(fmt::format("Unsupported value type: {}", toString(descriptor.type())));
    }
    return it->second();
}

}  // namespace diff_compare
