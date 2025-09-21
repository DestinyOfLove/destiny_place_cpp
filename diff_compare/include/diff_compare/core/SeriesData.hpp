#pragma once

#include <boost/utility/string_view.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesData {
public:
    using ViewType = boost::string_view;

    SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values);

    SeriesData(SeriesDescriptor descriptor,
               std::vector<ViewType> views,
               std::shared_ptr<void> backing_store = nullptr);

    SeriesData(SeriesDescriptor descriptor,
               std::vector<ViewType> views,
               std::vector<long long> ints,
               std::shared_ptr<void> backing_store = nullptr);

    SeriesData(SeriesDescriptor descriptor, std::vector<long long> ints);

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    const std::vector<std::string>& values() const;

    const std::vector<ViewType>& views() const;

    const std::vector<long long>& integers() const noexcept { return int_values_; }

    bool hasIntegers() const noexcept { return !int_values_.empty(); }

    std::size_t size() const noexcept;

    const std::string& valueAt(std::size_t index) const;

private:
    SeriesDescriptor descriptor_;
    std::shared_ptr<void> backing_store_;

    mutable std::vector<std::string> string_values_;
    mutable std::vector<ViewType> view_values_;
    mutable std::vector<long long> int_values_;

    void ensureStrings() const;
    void ensureViews() const;
};

}  // namespace diff_compare
