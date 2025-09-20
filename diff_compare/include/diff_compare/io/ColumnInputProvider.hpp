#pragma once

#include <memory>
#include <string>

#include "diff_compare/core/SeriesData.hpp"
#include "diff_compare/io/ColumnParser.hpp"

namespace diff_compare {

class ColumnInputProvider {
public:
    virtual ~ColumnInputProvider() = default;
    virtual SeriesData readColumn(const std::string& path) const = 0;
};

class TxtColumnInputProvider : public ColumnInputProvider {
public:
    explicit TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser);

    SeriesData readColumn(const std::string& path) const override;

private:
    std::shared_ptr<const ColumnParser> parser_;
};

}  // namespace diff_compare
