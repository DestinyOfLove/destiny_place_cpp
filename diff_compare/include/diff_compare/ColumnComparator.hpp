#pragma once

#include "diff_compare/ColumnData.hpp"
#include "diff_compare/ColumnDiff.hpp"

namespace diff_compare {

class ColumnComparator {
public:
    virtual ~ColumnComparator() = default;
    virtual ColumnDiff compare(const ColumnData& lhs, const ColumnData& rhs) const = 0;
};

class IntColumnComparator : public ColumnComparator {
public:
    ColumnDiff compare(const ColumnData& lhs, const ColumnData& rhs) const override;
};

class StringColumnComparator : public ColumnComparator {
public:
    ColumnDiff compare(const ColumnData& lhs, const ColumnData& rhs) const override;
};

}  // namespace diff_compare
