#include "diff_compare/io/ColumnInputProvider.hpp"

#include <fmt/core.h>

#include <boost/utility/string_view.hpp>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#endif

#include <fstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnParser.hpp"

namespace diff_compare {

namespace {

#if !defined(_WIN32)
class MemoryMappedFile {
public:
    explicit MemoryMappedFile(std::string path) : path_(std::move(path)), fd_(-1), size_(0), data_(nullptr) {
        fd_ = ::open(path_.c_str(), O_RDONLY);
        if (fd_ < 0) {
            throw std::runtime_error(fmt::format("Failed to open input file: {}", path_));
        }

        struct stat st{};
        if (::fstat(fd_, &st) != 0) {
            const int error = errno;
            ::close(fd_);
            fd_ = -1;
            throw std::runtime_error(fmt::format("Failed to stat input file {}: {}", path_, std::strerror(error)));
        }

        if (st.st_size <= 0) {
            ::close(fd_);
            fd_ = -1;
            throw std::invalid_argument(fmt::format("Input file {} missing header", path_));
        }

        size_ = static_cast<std::size_t>(st.st_size);
        void* address = ::mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
        if (address == MAP_FAILED) {
            const int error = errno;
            ::close(fd_);
            fd_ = -1;
            throw std::runtime_error(fmt::format("Failed to mmap {}: {}", path_, std::strerror(error)));
        }

        data_ = static_cast<const char*>(address);
    }

    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

    ~MemoryMappedFile() {
        if (data_) {
            ::munmap(const_cast<char*>(data_), size_);
        }
        if (fd_ >= 0) {
            ::close(fd_);
        }
    }

    const char* data() const { return data_; }

    std::size_t size() const { return size_; }

private:
    std::string path_;
    int fd_;
    std::size_t size_;
    const char* data_;
};

const char* findLineEnd(const char* current, const char* end) {
    const void* newline = std::memchr(current, '\n', static_cast<std::size_t>(end - current));
    return newline ? static_cast<const char*>(newline) : end;
}

SeriesData readSeriesWithMmap(const std::string& path, const ColumnParser& parser) {
    (void)parser;  // parser retained for signature symmetry
    auto mapped = std::make_shared<MemoryMappedFile>(path);
    const char* begin = mapped->data();
    const char* end = begin + mapped->size();

    const char* header_end = findLineEnd(begin, end);
    boost::string_view header_view(begin, static_cast<std::size_t>(header_end - begin));
    header_view = SimpleColumnParser::trimView(header_view);
    if (header_view.empty()) {
        throw std::invalid_argument(fmt::format("Input file {} missing header", path));
    }

    std::string header(header_view.data(), header_view.size());
    const ValueType type = valueTypeFromHeader(header);

    std::vector<boost::string_view> rows;
    const char* cursor = header_end;
    if (cursor < end && *cursor == '\n') {
        ++cursor;
    }

    while (cursor < end) {
        const char* line_end = findLineEnd(cursor, end);
        boost::string_view raw(cursor, static_cast<std::size_t>(line_end - cursor));
        const boost::string_view trimmed = SimpleColumnParser::trimView(raw);
        if (!trimmed.empty()) {
            rows.emplace_back(trimmed);
        }
        if (line_end == end) {
            break;
        }
        cursor = line_end + 1;
    }

    SeriesDescriptor descriptor(header, type);
    std::shared_ptr<void> backing = std::static_pointer_cast<void>(mapped);
    return SeriesData(std::move(descriptor), std::move(backing), std::move(rows));
}
#endif

SeriesData readSeriesWithParser(const std::string& path, const ColumnParser& parser) {
    std::ifstream input(path.c_str());
    if (!input.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open input file: {}", path));
    }
    return parser.parse(input);
}

}  // namespace

TxtColumnInputProvider::TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser)
    : parser_(std::move(parser)) {
    if (!parser_) {
        throw std::invalid_argument(fmt::format("TxtColumnInputProvider requires a non-null parser"));
    }
}

SeriesData TxtColumnInputProvider::readSeries(const std::string& path) const {
#if defined(_WIN32)
    return readSeriesWithParser(path, *parser_);
#else
    try {
        return readSeriesWithMmap(path, *parser_);
    } catch (const std::exception&) {
        return readSeriesWithParser(path, *parser_);
    }
#endif
}

}  // namespace diff_compare
