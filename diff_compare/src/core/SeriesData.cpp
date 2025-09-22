#include "diff_compare/core/SeriesData.hpp"

#include <fmt/core.h>

#include <stdexcept>
#include <utility>

namespace diff_compare {

SeriesData::SeriesData(SeriesDescriptor descriptor,
                       std::shared_ptr<void> backing_store,
                       std::shared_ptr<std::vector<std::string>> owned_strings,
                       std::vector<ViewType> views,
                       std::vector<long long> integers)
    : descriptor_(std::move(descriptor)),
      backing_store_(std::move(backing_store)),
      owned_strings_(std::move(owned_strings)),
      view_values_(std::move(views)),
      int_values_(std::move(integers)) {}

SeriesData SeriesData::fromStringValues(SeriesDescriptor descriptor, std::vector<std::string> values) {
    auto owned = std::make_shared<std::vector<std::string>>(std::move(values));
    std::vector<ViewType> views;
    views.reserve(owned->size());
    for (const std::string& value : *owned) {
        views.emplace_back(value.data(), value.size());
    }
    return SeriesData(std::move(descriptor), nullptr, std::move(owned), std::move(views), {});
}

SeriesData SeriesData::fromViews(SeriesDescriptor descriptor,
                                 std::vector<ViewType> views,
                                 std::shared_ptr<void> backing_store) {
    return SeriesData(std::move(descriptor), std::move(backing_store), nullptr, std::move(views), {});
}

SeriesData SeriesData::fromViewsAndInts(SeriesDescriptor descriptor,
                                        std::vector<ViewType> views,
                                        std::vector<long long> integers,
                                        std::shared_ptr<void> backing_store) {
    if (views.size() != integers.size()) {
        throw std::invalid_argument("SeriesData::fromViewsAndInts requires views and integers of the same size");
    }
    return SeriesData(std::move(descriptor), std::move(backing_store), nullptr, std::move(views),
                      std::move(integers));
}

const std::vector<long long>& SeriesData::integers() const {
    if (int_values_.empty()) {
        throw std::logic_error("SeriesData: integer buffer requested but not available");
    }
    return int_values_;
}

std::size_t SeriesData::size() const noexcept {
    if (!view_values_.empty()) {
        return view_values_.size();
    }
    return int_values_.size();
}

std::string SeriesData::valueAt(std::size_t index) const {
    if (!view_values_.empty()) {
        if (index >= view_values_.size()) {
            throw std::out_of_range(fmt::format("SeriesData index out of range"));
        }
        const auto view = view_values_[index];
        return std::string(view.data(), view.size());
    }
    if (!int_values_.empty()) {
        if (index >= int_values_.size()) {
            throw std::out_of_range(fmt::format("SeriesData index out of range"));
        }
        return std::to_string(int_values_[index]);
    }
    throw std::out_of_range(fmt::format("SeriesData index out of range"));
}

}  // namespace diff_compare
