#include "diff_compare/io/ColumnInputProvider.hpp"

#include <fmt/core.h>

#include <boost/utility/string_view.hpp>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>
#include <utility>
#include <vector>

#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnParser.hpp"

namespace diff_compare {

namespace {

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

long long parseIntegerView(boost::string_view view, bool& valid) {
    valid = false;
    if (view.empty()) {
        return 0;
    }

    std::size_t index = 0;
    bool negative = false;
    if (view[0] == '-' || view[0] == '+') {
        negative = view[0] == '-';
        index = 1;
        if (index == view.size()) {
            return 0;
        }
    }

    long long value = 0;
    for (; index < view.size(); ++index) {
        const char ch = view[index];
        if (ch < '0' || ch > '9') {
            return 0;
        }
        value = value * 10 + (ch - '0');
    }
    valid = true;
    return negative ? -value : value;
}

SeriesData readSeriesWithMmap(const std::string& path) {
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
    std::vector<long long> ints;
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
            if (type == ValueType::Integer) {
                bool ok = false;
                long long value = parseIntegerView(trimmed, ok);
                if (!ok) {
                    ints.clear();
                } else if (ints.size() == rows.size() - 1) {
                    ints.push_back(value);
                }
            }
        }
        if (line_end == end) {
            break;
        }
        cursor = line_end + 1;
    }

    SeriesDescriptor descriptor(header, type);
    std::shared_ptr<void> backing = std::static_pointer_cast<void>(mapped);
    if (type == ValueType::Integer && ints.size() == rows.size()) {
        return SeriesData(std::move(descriptor), std::move(rows), std::move(ints), std::move(backing));
    }
    return SeriesData(std::move(descriptor), std::move(rows), std::move(backing));
}
}  // namespace

TxtColumnInputProvider::TxtColumnInputProvider(std::shared_ptr<const ColumnParser> parser)
    : parser_(std::move(parser)) {
    if (!parser_) {
        throw std::invalid_argument(fmt::format("TxtColumnInputProvider requires a non-null parser"));
    }
}

SeriesData TxtColumnInputProvider::readSeries(const std::string& path) const {
    return readSeriesWithMmap(path);
}

}  // namespace diff_compare
