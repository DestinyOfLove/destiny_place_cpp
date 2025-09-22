#include "diff_compare/core/SeriesData.hpp"

#include <fmt/core.h>

#include <stdexcept>
#include <utility>

namespace diff_compare {

SeriesData::SeriesData(SeriesDescriptor descriptor, SeriesKind kind)
    : descriptor_(std::move(descriptor)), kind_(kind) {}

const std::vector<SeriesData::ViewType>& SeriesData::views() const {
    throw std::logic_error("SeriesData: string views not available for this kind");
}

NumericView SeriesData::numeric() const {
    throw std::logic_error("SeriesData: numeric view not available for this kind");
}

std::string SeriesData::valueAt(std::size_t index) const {
    return doValueAt(index);
}

StringSeriesData::StringSeriesData(SeriesDescriptor descriptor,
                                   std::shared_ptr<std::vector<std::string>> owned_strings,
                                   std::shared_ptr<void> backing_store,
                                   std::vector<ViewType> views)
    : SeriesData(std::move(descriptor), SeriesKind::String),
      owned_strings_(std::move(owned_strings)),
      backing_store_(std::move(backing_store)),
      views_(std::move(views)) {}

std::shared_ptr<StringSeriesData> StringSeriesData::fromValues(SeriesDescriptor descriptor,
                                                               std::vector<std::string> values) {
    auto owned = std::make_shared<std::vector<std::string>>(std::move(values));
    std::vector<ViewType> views;
    views.reserve(owned->size());
    for (const std::string& value : *owned) {
        views.emplace_back(value.data(), value.size());
    }
    return std::shared_ptr<StringSeriesData>(
        new StringSeriesData(std::move(descriptor), std::move(owned), nullptr, std::move(views)));
}

std::shared_ptr<StringSeriesData> StringSeriesData::fromViews(SeriesDescriptor descriptor,
                                                              std::vector<ViewType> views,
                                                              std::shared_ptr<void> backing_store) {
    return std::shared_ptr<StringSeriesData>(
        new StringSeriesData(std::move(descriptor), nullptr, std::move(backing_store), std::move(views)));
}

std::string StringSeriesData::doValueAt(std::size_t index) const {
    if (index >= views_.size()) {
        throw std::out_of_range(fmt::format("SeriesData index out of range"));
    }
    const auto view = views_[index];
    return std::string(view.data(), view.size());
}

NumericSeriesData::NumericSeriesData(SeriesDescriptor descriptor, std::vector<long long> values)
    : SeriesData(std::move(descriptor), SeriesKind::Numeric), values_(std::move(values)) {}

std::shared_ptr<NumericSeriesData> NumericSeriesData::fromInt64Values(SeriesDescriptor descriptor,
                                                                      std::vector<long long> values) {
    return std::shared_ptr<NumericSeriesData>(new NumericSeriesData(std::move(descriptor), std::move(values)));
}

NumericView NumericSeriesData::numeric() const noexcept {
    return NumericView::fromInt64(Int64Span(values_.data(), values_.size()));
}

std::string NumericSeriesData::doValueAt(std::size_t index) const {
    if (index >= values_.size()) {
        throw std::out_of_range(fmt::format("SeriesData index out of range"));
    }
    return std::to_string(values_[index]);
}

}  // namespace diff_compare
