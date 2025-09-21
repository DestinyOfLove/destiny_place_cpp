#include "diff_compare/core/SeriesData.hpp"

#include <fmt/core.h>

#include <stdexcept>
#include <utility>

namespace diff_compare {

SeriesData::SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)),
      backing_store_(),
      string_values_(std::move(values)),
      view_values_(),
      int_values_() {}

SeriesData::SeriesData(SeriesDescriptor descriptor,
                       std::vector<ViewType> views,
                       std::shared_ptr<void> backing_store)
    : descriptor_(std::move(descriptor)),
      backing_store_(std::move(backing_store)),
      string_values_(),
      view_values_(std::move(views)),
      int_values_() {}

SeriesData::SeriesData(SeriesDescriptor descriptor,
                       std::vector<ViewType> views,
                       std::vector<long long> ints,
                       std::shared_ptr<void> backing_store)
    : descriptor_(std::move(descriptor)),
      backing_store_(std::move(backing_store)),
      string_values_(),
      view_values_(std::move(views)),
      int_values_(std::move(ints)) {}

SeriesData::SeriesData(SeriesDescriptor descriptor, std::vector<long long> ints)
    : descriptor_(std::move(descriptor)),
      backing_store_(),
      string_values_(),
      view_values_(),
      int_values_(std::move(ints)) {}

void SeriesData::ensureStrings() const {
    if (!string_values_.empty()) {
        return;
    }

    if (!view_values_.empty()) {
        string_values_.reserve(view_values_.size());
        for (const ViewType& view : view_values_) {
            string_values_.emplace_back(view.data(), view.size());
        }
        return;
    }

    if (!int_values_.empty()) {
        string_values_.reserve(int_values_.size());
        for (long long value : int_values_) {
            string_values_.emplace_back(std::to_string(value));
        }
    }
}

void SeriesData::ensureViews() const {
    if (!view_values_.empty()) {
        return;
    }

    if (!string_values_.empty()) {
        view_values_.reserve(string_values_.size());
        for (const std::string& value : string_values_) {
            view_values_.emplace_back(value.data(), value.size());
        }
        return;
    }

    if (!int_values_.empty()) {
        // materialize numeric strings once to expose views
        ensureStrings();
        view_values_.reserve(string_values_.size());
        for (const std::string& value : string_values_) {
            view_values_.emplace_back(value.data(), value.size());
        }
    }
}

const std::vector<std::string>& SeriesData::values() const {
    ensureStrings();
    return string_values_;
}

const std::vector<SeriesData::ViewType>& SeriesData::views() const {
    ensureViews();
    return view_values_;
}

std::size_t SeriesData::size() const noexcept {
    if (!int_values_.empty()) {
        return int_values_.size();
    }
    if (!view_values_.empty()) {
        return view_values_.size();
    }
    return string_values_.size();
}

const std::string& SeriesData::valueAt(std::size_t index) const {
    const auto& vals = values();
    if (index >= vals.size()) {
        throw std::out_of_range(fmt::format("SeriesData index out of range"));
    }
    return vals[index];
}

}  // namespace diff_compare
