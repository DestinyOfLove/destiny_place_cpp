#pragma once

#include <boost/utility/string_view.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"

namespace diff_compare {

enum class SeriesKind {
    String,
    Numeric
};

enum class NumericKind {
    Int64,
    Double
};

template <typename T>
struct SeriesSpan {
    SeriesSpan() : data(nullptr), size(0) {}

    SeriesSpan(const T* ptr, std::size_t length) : data(ptr), size(length) {}

    const T& operator[](std::size_t index) const { return data[index]; }

    const T* begin() const { return data; }

    const T* end() const { return data ? data + size : nullptr; }

    const T* data;
    std::size_t size;
};

using Int64Span = SeriesSpan<long long>;
using DoubleSpan = SeriesSpan<double>;

struct NumericView {
    NumericKind kind;
    Int64Span asInt64;
    DoubleSpan asDouble;

    static NumericView fromInt64(Int64Span values) { return NumericView{NumericKind::Int64, values, {}}; }
};

class SeriesData {
public:
    using ViewType = boost::string_view;

    virtual ~SeriesData() = default;

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    SeriesKind kind() const noexcept { return kind_; }

    virtual std::size_t size() const noexcept = 0;

    virtual const std::vector<ViewType>& views() const;
    virtual NumericView numeric() const;

    std::string valueAt(std::size_t index) const;

protected:
    SeriesData(SeriesDescriptor descriptor, SeriesKind kind);

private:
    virtual std::string doValueAt(std::size_t index) const = 0;

    SeriesDescriptor descriptor_;
    SeriesKind kind_;
};

class StringSeriesData : public SeriesData {
public:
    static std::shared_ptr<StringSeriesData> fromValues(SeriesDescriptor descriptor, std::vector<std::string> values);
    static std::shared_ptr<StringSeriesData> fromViews(SeriesDescriptor descriptor,
                                                       std::vector<ViewType> views,
                                                       std::shared_ptr<void> backing_store = nullptr);

    std::size_t size() const noexcept override { return views_.size(); }

    const std::vector<ViewType>& views() const noexcept override { return views_; }

private:
    StringSeriesData(SeriesDescriptor descriptor,
                     std::shared_ptr<std::vector<std::string>> owned_strings,
                     std::shared_ptr<void> backing_store,
                     std::vector<ViewType> views);

    std::string doValueAt(std::size_t index) const override;

    std::shared_ptr<std::vector<std::string>> owned_strings_;
    std::shared_ptr<void> backing_store_;
    std::vector<ViewType> views_;
};

class NumericSeriesData : public SeriesData {
public:
    static std::shared_ptr<NumericSeriesData> fromInt64Values(SeriesDescriptor descriptor,
                                                              std::vector<long long> values);

    std::size_t size() const noexcept override { return values_.size(); }

    NumericView numeric() const noexcept override;

private:
    explicit NumericSeriesData(SeriesDescriptor descriptor, std::vector<long long> values);

    std::string doValueAt(std::size_t index) const override;

    std::vector<long long> values_;
};

using SeriesDataPtr = std::shared_ptr<SeriesData>;

}  // namespace diff_compare
