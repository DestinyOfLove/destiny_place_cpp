#include "diff_compare/core/SeriesData.hpp"

#include <fmt/core.h>

#include <stdexcept>
#include <utility>

namespace diff_compare {

namespace {

class VectorSeriesCursor : public SeriesCursor {
public:
    explicit VectorSeriesCursor(const std::vector<std::string>* values) : values_(values), index_(0) {}

    bool next(std::string& value) override {
        if (!values_ || index_ >= values_->size()) {
            return false;
        }
        value = (*values_)[index_++];
        return true;
    }

    void reset() override { index_ = 0; }

private:
    const std::vector<std::string>* values_;
    std::size_t index_;
};

}  // namespace

SeriesData::SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values)
    : descriptor_(std::move(descriptor)),
      cursor_factory_(),
      backing_store_(),
      materialized_values_(std::move(values)),
      view_values_(),
      materialized_(true),
      views_valid_(false),
      size_hint_(materialized_values_.size()) {}

SeriesData::SeriesData(SeriesDescriptor descriptor,
                       std::shared_ptr<const SeriesCursorFactory> cursor_factory,
                       std::size_t size_hint)
    : descriptor_(std::move(descriptor)),
      cursor_factory_(std::move(cursor_factory)),
      backing_store_(),
      materialized_values_(),
      view_values_(),
      int_values_(),
      materialized_(false),
      views_valid_(false),
      size_hint_(size_hint) {}

SeriesData::SeriesData(SeriesDescriptor descriptor, std::shared_ptr<void> backing_store, std::vector<ViewType> views)
    : descriptor_(std::move(descriptor)),
      cursor_factory_(),
      backing_store_(std::move(backing_store)),
      materialized_values_(),
      view_values_(std::move(views)),
      int_values_(),
      materialized_(false),
      views_valid_(true),
      size_hint_(view_values_.size()) {}

SeriesData::SeriesData(SeriesDescriptor descriptor, std::shared_ptr<void> backing_store, std::vector<long long> ints)
    : descriptor_(std::move(descriptor)),
      cursor_factory_(),
      backing_store_(std::move(backing_store)),
      materialized_values_(),
      view_values_(),
      int_values_(std::move(ints)),
      materialized_(false),
      views_valid_(true),
      size_hint_(int_values_.size()) {}

void SeriesData::ensureMaterialized() const {
    if (materialized_) {
        return;
    }

    materialized_values_.clear();
    if (cursor_factory_) {
        auto cursor = cursor_factory_->create();
        if (cursor) {
            std::string value;
            while (cursor->next(value)) {
                materialized_values_.push_back(value);
            }
        }
    } else if (views_valid_) {
        materialized_values_.reserve(view_values_.size());
        for (const ViewType& view : view_values_) {
            materialized_values_.emplace_back(view.data(), view.size());
        }
    }

    size_hint_ = materialized_values_.size();
    materialized_ = true;
    views_valid_ = false;
}

void SeriesData::ensureViews() const {
    if (views_valid_) {
        return;
    }

    if (materialized_) {
        view_values_.clear();
        view_values_.reserve(materialized_values_.size());
        for (const std::string& value : materialized_values_) {
            view_values_.emplace_back(value.data(), value.size());
        }
        views_valid_ = true;
        return;
    }

    if (cursor_factory_) {
        ensureMaterialized();
        ensureViews();
        return;
    }

    view_values_.clear();
    views_valid_ = true;
}

void SeriesData::ensureIntegers() const {
    if (!int_values_.empty()) {
        return;
    }

    if (descriptor_.type() != ValueType::Integer) {
        return;
    }

    ensureMaterialized();
    if (materialized_values_.empty()) {
        return;
    }

    int_values_.reserve(materialized_values_.size());
    for (const std::string& str : materialized_values_) {
        boost::string_view view(str);
        long long value = 0;
        std::size_t index = 0;
        bool negative = false;
        if (!view.empty() && (view[0] == '-' || view[0] == '+')) {
            negative = view[0] == '-';
            index = 1;
        }
        for (; index < view.size(); ++index) {
            if (view[index] < '0' || view[index] > '9') {
                int_values_.clear();
                return;
            }
            value = value * 10 + (view[index] - '0');
        }
        int_values_.push_back(negative ? -value : value);
    }
}

const std::vector<std::string>& SeriesData::values() const {
    ensureMaterialized();
    return materialized_values_;
}

const std::vector<SeriesData::ViewType>& SeriesData::views() const {
    ensureViews();
    return view_values_;
}

std::size_t SeriesData::size() const noexcept {
    if (views_valid_) {
        return view_values_.size();
    }
    if (!int_values_.empty()) {
        return int_values_.size();
    }
    if (materialized_) {
        return materialized_values_.size();
    }
    return size_hint_;
}

const std::string& SeriesData::valueAt(std::size_t index) const {
    const auto& vals = values();
    if (index >= vals.size()) {
        throw std::out_of_range(fmt::format("SeriesData index out of range"));
    }
    return vals[index];
}

std::unique_ptr<SeriesCursor> SeriesData::cursor() const {
    if (cursor_factory_) {
        return cursor_factory_->create();
    }
    ensureMaterialized();
    return std::unique_ptr<SeriesCursor>(new VectorSeriesCursor(&materialized_values_));
}

const std::vector<long long>& SeriesData::integers() const {
    return int_values_;
}

}  // namespace diff_compare
