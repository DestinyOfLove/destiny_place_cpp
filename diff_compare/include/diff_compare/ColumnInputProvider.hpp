#pragma once

#include <memory>
#include <string>

#include "diff_compare/ColumnParser.hpp"

namespace diff_compare {

class ColumnInputProvider {
public:
    virtual ~ColumnInputProvider() = default;
    virtual ColumnData readColumn(const std::string& path) const = 0;
};

class TxtColumnInputProvider : public ColumnInputProvider {
public:
    explicit TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser);

    ColumnData readColumn(const std::string& path) const override;

private:
    std::shared_ptr<const ColumnParser> parser_;
};

}  // namespace diff_compare
