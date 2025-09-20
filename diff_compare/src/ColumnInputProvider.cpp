#include "diff_compare/ColumnInputProvider.hpp"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace diff_compare {

TxtColumnInputProvider::TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser)
    : parser_(std::move(parser)) {
    if (!parser_) {
        throw std::invalid_argument("TxtColumnInputProvider requires a non-null parser");
    }
}

SeriesData TxtColumnInputProvider::readColumn(const std::string& path) const {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open input file: " + path);
    }
    return parser_->parse(input);
}

}  // namespace diff_compare
