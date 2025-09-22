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

    static SeriesData fromStringValues(SeriesDescriptor descriptor, std::vector<std::string> values);
    static SeriesData fromViews(SeriesDescriptor descriptor,
                                std::vector<ViewType> views,
                                std::shared_ptr<void> backing_store = nullptr);
    static SeriesData fromViewsAndInts(SeriesDescriptor descriptor,
                                       std::vector<ViewType> views,
                                       std::vector<long long> integers,
                                       std::shared_ptr<void> backing_store = nullptr);

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    const std::vector<ViewType>& views() const noexcept { return view_values_; }
    const std::vector<long long>& integers() const;

    bool hasIntegers() const noexcept { return !int_values_.empty(); }

    std::size_t size() const noexcept;

    std::string valueAt(std::size_t index) const;

private:
    SeriesDescriptor descriptor_;
    std::shared_ptr<void> backing_store_;
    std::shared_ptr<std::vector<std::string>> owned_strings_;
    std::vector<ViewType> view_values_;
    std::vector<long long> int_values_;

    SeriesData(SeriesDescriptor descriptor,
               std::shared_ptr<void> backing_store,
               std::shared_ptr<std::vector<std::string>> owned_strings,
               std::vector<ViewType> views,
               std::vector<long long> integers);
};

}  // namespace diff_compare
