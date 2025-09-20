#include "diff_compare/io/DiffOutputWriter.hpp"

#include <fstream>
#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

TxtDiffOutputWriter::TxtDiffOutputWriter(std::shared_ptr<const OutputFormatter> formatter)
    : formatter_(std::move(formatter)) {
    if (!formatter_) {
        throw std::invalid_argument(fmt::format("TxtDiffOutputWriter requires a non-null formatter"));
    }
}

void TxtDiffOutputWriter::writeSeries(const SeriesDiff& diff, const std::string& path) const {
    std::ofstream output(path);
    if (!output.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open output file: {}", path));
    }
    const auto lines = formatter_->format(diff);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        output << lines[i];
        if (i + 1 < lines.size()) {
            output << '\n';
        }
    }
    if (!output) {
        throw std::runtime_error(fmt::format("Failed to write output file: {}", path));
    }
}

}  // namespace diff_compare
