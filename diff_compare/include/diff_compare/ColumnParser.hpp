#pragma once

#include <istream>
#include <memory>
#include <string>

#include "diff_compare/ColumnData.hpp"

namespace diff_compare {

class ColumnParser {
public:
    virtual ~ColumnParser() = default;
    virtual ColumnData parse(std::istream& input) const = 0;
};

class SimpleColumnParser : public ColumnParser {
public:
    ColumnData parse(std::istream& input) const override;
};

}  // namespace diff_compare
