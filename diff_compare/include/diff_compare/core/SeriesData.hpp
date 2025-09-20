#pragma once

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
    SeriesData(SeriesDescriptor descriptor, std::vector<std::string> values);

    SeriesData(SeriesDescriptor descriptor,
               std::shared_ptr<const SeriesCursorFactory> cursor_factory,
               std::size_t size_hint = 0);

    const SeriesDescriptor& descriptor() const noexcept { return descriptor_; }

    const std::vector<std::string>& values() const;

    std::size_t size() const noexcept;

    const std::string& valueAt(std::size_t index) const;

    std::unique_ptr<SeriesCursor> cursor() const;

    bool hasCursor() const noexcept { return static_cast<bool>(cursor_factory_); }

private:
    SeriesDescriptor descriptor_;
    std::shared_ptr<const SeriesCursorFactory> cursor_factory_;
    mutable std::vector<std::string> materialized_values_;
    mutable bool materialized_;
    mutable std::size_t size_hint_;

    void ensureMaterialized() const;
};

}  // namespace diff_compare
