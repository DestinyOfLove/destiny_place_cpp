#pragma once

#include <istream>
#include <memory>
#include <string>

#include "diff_compare/core/SeriesData.hpp"

namespace diff_compare {

class ColumnParser {
public:
    virtual ~ColumnParser() = default;
    virtual SeriesData parse(std::istream& input) const = 0;
};

class SimpleColumnParser : public ColumnParser {
public:
    SeriesData parse(std::istream& input) const override;

    static std::string sanitizeLine(std::string line);
};

}  // namespace diff_compare
