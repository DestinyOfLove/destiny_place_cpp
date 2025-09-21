#pragma once

#include <boost/utility/string_view.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"

namespace diff_compare {

class SeriesCursor {
public:
    virtual ~SeriesCursor() = default;
    virtual bool next(std::string& value) = 0;
    virtual void reset() = 0;
};

class SeriesCursorFactory {
public:
    virtual ~SeriesCursorFactory() = default;
    virtual std::unique_ptr<SeriesCursor> create() const = 0;
};

class SeriesData {
public:
    using ViewType = boost::string_view;

    SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values);

    SeriesData(SeriesDescriptor descriptor,
               std::shared_ptr<const SeriesCursorFactory> cursor_factory,
               std::size_t size_hint = 0);

    SeriesData(SeriesDescriptor descriptor, std::shared_ptr<void> backing_store, std::vector<ViewType> views);

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    const std::vector<std::string>& values() const;

    const std::vector<ViewType>& views() const;

    std::size_t size() const noexcept;

    const std::string& valueAt(std::size_t index) const;

    std::unique_ptr<SeriesCursor> cursor() const;

    bool hasCursor() const noexcept { return static_cast<bool>(cursor_factory_); }

private:
    SeriesDescriptor descriptor_;
    std::shared_ptr<const SeriesCursorFactory> cursor_factory_;
    std::shared_ptr<void> backing_store_;

    mutable std::vector<std::string> materialized_values_;
    mutable std::vector<ViewType> view_values_;
    mutable bool materialized_;
    mutable bool views_valid_;
    mutable std::size_t size_hint_;

    void ensureMaterialized() const;
    void ensureViews() const;
};

}  // namespace diff_compare
