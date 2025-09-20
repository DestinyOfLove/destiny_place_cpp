#include "diff_compare/io/ColumnInputProvider.hpp"

#include <fmt/core.h>

#include <fstream>
#include <stdexcept>
#include <utility>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnParser.hpp"

namespace diff_compare {

namespace {

class TxtSeriesCursor : public SeriesCursor {
public:
    TxtSeriesCursor(std::string path, std::string header)
        : path_(std::move(path)), expected_header_(std::move(header)) {
        reopen();
    }

    bool next(std::string& value) override {
        while (std::getline(stream_, line_buffer_)) {
            sanitized_buffer_ = SimpleColumnParser::sanitizeLine(std::move(line_buffer_));
            if (sanitized_buffer_.empty()) {
                continue;
            }
            value.swap(sanitized_buffer_);
            line_buffer_.swap(sanitized_buffer_);
            return true;
        }
        return false;
    }

    void reset() override { reopen(); }

private:
    void reopen() {
        stream_.close();
        stream_.clear();
        stream_.open(path_.c_str());
        if (!stream_.is_open()) {
            throw std::runtime_error(fmt::format("Failed to open input file: {}", path_));
        }
        std::string headerLine;
        if (!std::getline(stream_, headerLine)) {
            throw std::runtime_error(fmt::format("Input file {} missing header", path_));
        }
        headerLine = SimpleColumnParser::sanitizeLine(std::move(headerLine));
        if (headerLine != expected_header_) {
            throw std::runtime_error(fmt::format(
                "Column header mismatch in {}: expected '{}' but got '{}'", path_, expected_header_, headerLine));
        }
    }

    std::string path_;
    std::string expected_header_;
    std::ifstream stream_;
    std::string line_buffer_;
    std::string sanitized_buffer_;
};

class TxtSeriesCursorFactory : public SeriesCursorFactory {
public:
    TxtSeriesCursorFactory(std::string path, std::string header) : path_(std::move(path)), header_(std::move(header)) {}

    std::unique_ptr<SeriesCursor> create() const override {
        return std::unique_ptr<SeriesCursor>(new TxtSeriesCursor(path_, header_));
    }

private:
    std::string path_;
    std::string header_;
};

}  // namespace

TxtColumnInputProvider::TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser)
    : parser_(std::move(parser)) {
    if (!parser_) {
        throw std::invalid_argument(fmt::format("TxtColumnInputProvider requires a non-null parser"));
    }
}

SeriesData TxtColumnInputProvider::readSeries(const std::string& path) const {
    std::ifstream input(path.c_str());
    if (!input.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open input file: {}", path));
    }

    std::string header;
    if (!std::getline(input, header)) {
        throw std::invalid_argument(fmt::format("Input column missing header"));
    }
    header = SimpleColumnParser::sanitizeLine(std::move(header));
    const ValueType type = valueTypeFromHeader(header);

    SeriesDescriptor descriptor(header, type);
    auto factory = std::make_shared<TxtSeriesCursorFactory>(path, header);
    return SeriesData(std::move(descriptor), factory);
}

}  // namespace diff_compare
