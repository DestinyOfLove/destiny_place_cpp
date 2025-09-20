#include "diff_compare/io/ColumnInputProvider.hpp"

#include <fstream>
#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

TxtColumnInputProvider::TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser)
    : parser_(std::move(parser)) {
    if (!parser_) {
        throw std::invalid_argument(fmt::format("TxtColumnInputProvider requires a non-null parser"));
    }
}

SeriesData TxtColumnInputProvider::readSeries(const std::string& path) const {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open input file: {}", path));
    }
    return parser_->parse(input);
}

}  // namespace diff_compare
