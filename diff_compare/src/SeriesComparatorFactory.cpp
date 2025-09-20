#include "diff_compare/SeriesComparatorFactory.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

#include "diff_compare/ValueType.hpp"

namespace diff_compare {

namespace {

std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder>& comparatorRegistry() {
    static std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder> registry = [] {
        std::map<ValueType, SeriesComparatorFactory::ComparatorBuilder> initial;
        initial.emplace(ValueType::Integer,
                        [] { return std::unique_ptr<SeriesComparator>(new NumericSeriesComparator()); });
        initial.emplace(ValueType::String,
                        [] { return std::unique_ptr<SeriesComparator>(new TextSeriesComparator()); });
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
    comparatorRegistry()[type] = std::move(builder);
}

std::unique_ptr<SeriesComparator> SeriesComparatorFactory::create(const SeriesDescriptor& descriptor) const {
    std::lock_guard<std::mutex> lock(registryMutex());
    auto& registry = comparatorRegistry();
    const auto it = registry.find(descriptor.type());
    if (it == registry.end()) {
        throw std::invalid_argument("Unsupported value type: " + toString(descriptor.type()));
    }
    return it->second();
}

}  // namespace diff_compare
