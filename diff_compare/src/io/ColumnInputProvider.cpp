#include "diff_compare/io/ColumnInputProvider.hpp"

#include <fmt/core.h>

#include <fstream>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnParser.hpp"

namespace diff_compare {

namespace {

class TxtSeriesCursor : public SeriesCursor {
public:
    TxtSeriesCursor(std::string path, std::string header)
        : path_(std::move(path)),
          expected_header_(std::move(header)),
          chunk_(kChunkSize),
          chunk_index_(0),
          chunk_size_(0) {
        reopen();
    }

    bool next(std::string& value) override {
        while (true) {
            if (chunk_index_ < chunk_size_) {
                value.swap(chunk_[chunk_index_]);
                chunk_[chunk_index_].clear();
                ++chunk_index_;
                return true;
            }
            if (!fillChunk()) {
                return false;
            }
        }
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
        chunk_index_ = 0;
        chunk_size_ = 0;
    }

    std::string path_;
    std::string expected_header_;
    std::ifstream stream_;
    std::string line_buffer_;
    std::vector<std::string> chunk_;
    std::size_t chunk_index_;
    std::size_t chunk_size_;

    static constexpr std::size_t kChunkSize = 1024;

    bool fillChunk() {
        chunk_size_ = 0;
        chunk_index_ = 0;
        while (chunk_size_ < kChunkSize && std::getline(stream_, line_buffer_)) {
            if (!line_buffer_.empty() && line_buffer_.back() == '\r') {
                line_buffer_.pop_back();
            }
            const std::string_view trimmed = SimpleColumnParser::trimView(line_buffer_);
            if (trimmed.empty()) {
                continue;
            }
            std::string& slot = chunk_[chunk_size_++];
            slot.assign(trimmed.begin(), trimmed.end());
        }
        line_buffer_.clear();
        return chunk_size_ > 0;
    }
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
