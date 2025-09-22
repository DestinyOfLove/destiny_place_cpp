#pragma once

#include <memory>
#include <string>

#include "diff_compare/core/SeriesData.hpp"
#include "diff_compare/io/ColumnParser.hpp"
#include "diff_compare/io/SeriesInputProvider.hpp"

namespace diff_compare {

class ColumnInputProvider : public SeriesInputProvider {
public:
    virtual ~ColumnInputProvider() = default;
    SeriesDataPtr readSeries(const std::string& path) const override = 0;
};

class TxtColumnInputProvider : public ColumnInputProvider {
public:
    explicit TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser);

    SeriesDataPtr readSeries(const std::string& path) const override;

private:
    std::shared_ptr<const ColumnParser> parser_;
};

}  // namespace diff_compare
