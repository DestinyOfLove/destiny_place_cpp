#pragma once

#include <memory>
#include <string>
#include <vector>

#include "diff_compare/ColumnDiff.hpp"

namespace diff_compare {

class OutputFormatter {
public:
    virtual ~OutputFormatter() = default;
    virtual std::vector<std::string> format(const ColumnDiff& diff) const = 0;
};

class PlainTextOutputFormatter : public OutputFormatter {
public:
    std::vector<std::string> format(const ColumnDiff& diff) const override;
};

}  // namespace diff_compare
