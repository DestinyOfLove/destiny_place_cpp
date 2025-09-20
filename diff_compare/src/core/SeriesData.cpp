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
      materialized_values_(std::move(values)),
      materialized_(true),
      size_hint_(materialized_values_.size()) {}

SeriesData::SeriesData(SeriesDescriptor descriptor,
                       std::shared_ptr<const SeriesCursorFactory> cursor_factory,
                       std::size_t size_hint)
    : descriptor_(std::move(descriptor)),
      cursor_factory_(std::move(cursor_factory)),
      materialized_values_(),
      materialized_(false),
      size_hint_(size_hint) {}

void SeriesData::ensureMaterialized() const {
    if (materialized_) {
        return;
    }
    materialized_values_.clear();
    if (!cursor_factory_) {
        materialized_ = true;
        return;
    }
    auto cursor = cursor_factory_->create();
    if (!cursor) {
        materialized_ = true;
        return;
    }
    std::string value;
    while (cursor->next(value)) {
        materialized_values_.push_back(value);
    }
    size_hint_ = materialized_values_.size();
    materialized_ = true;
}

const std::vector<std::string>& SeriesData::values() const {
    ensureMaterialized();
    return materialized_values_;
}

std::size_t SeriesData::size() const noexcept {
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
    return std::unique_ptr<SeriesCursor>(new VectorSeriesCursor(&materialized_values_));
}

}  // namespace diff_compare
