#include <benchmark/benchmark.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(__APPLE__)
#include <mach/mach.h>
#endif

#include <fmt/core.h>

#include "diff_compare/app/ColumnProcessingPipeline.hpp"
#include "diff_compare/core/SeriesComparatorFactory.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnInputProvider.hpp"
#include "diff_compare/io/ColumnParser.hpp"
#include "diff_compare/io/DiffOutputWriter.hpp"
#include "diff_compare/io/OutputFormatter.hpp"

namespace diff_compare {
namespace perf {

struct PerfScenario {
    std::string name;
    ValueType valueType;
    std::size_t rowCount;
    std::size_t stringLength;    // Only used for string scenarios.
    std::size_t mismatchStride;  // Every N rows introduce a mismatch; 0 means identical series.
};

struct TempFiles {
    std::string lhsPath;
    std::string rhsPath;
    std::string outputPath;

    ~TempFiles() {
        if (!lhsPath.empty()) {
            std::remove(lhsPath.c_str());
        }
        if (!rhsPath.empty()) {
            std::remove(rhsPath.c_str());
        }
        if (!outputPath.empty()) {
            std::remove(outputPath.c_str());
        }
    }
};

std::string TempDirectory() {
    const char* env = std::getenv("TMPDIR");
    if (env && *env) {
        return std::string(env);
    }
    env = std::getenv("TEMP");
    if (env && *env) {
        return std::string(env);
    }
    env = std::getenv("TMP");
    if (env && *env) {
        return std::string(env);
    }
    return std::string("/tmp");
}

std::string MakeTempFile(const std::string& prefix) {
    std::string pattern = TempDirectory();
    if (!pattern.empty() && pattern.back() != '/') {
        pattern.push_back('/');
    }
    pattern.append(prefix);
    pattern.append("XXXXXX");

    std::vector<char> path(pattern.begin(), pattern.end());
    path.push_back('\0');

    int fd = ::mkstemp(path.data());
    if (fd == -1) {
        throw std::runtime_error(fmt::format("mkstemp failed for {}: {}", pattern, std::strerror(errno)));
    }
    ::close(fd);
    return std::string(path.data());
}

std::string MakeHeader(ValueType valueType) {
    switch (valueType) {
        case ValueType::Integer:
            return "Int_Benchmark";
        case ValueType::String:
            return "Str_Benchmark";
        default:
            throw std::invalid_argument(
                fmt::format("Unsupported ValueType in benchmark: {}", static_cast<int>(valueType)));
    }
}

std::string MakeStringValue(std::size_t index, std::size_t length) {
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::size_t alphabetSize = sizeof(alphabet) - 1;
    std::string value;
    value.reserve(length);
    for (std::size_t i = 0; i < length; ++i) {
        value.push_back(alphabet[(index + i) % alphabetSize]);
    }
    return value;
}

void WriteIntegerColumns(const PerfScenario& scenario, const TempFiles& files) {
    const std::string header = MakeHeader(ValueType::Integer);
    std::ofstream lhs(files.lhsPath.c_str(), std::ios::trunc);
    std::ofstream rhs(files.rhsPath.c_str(), std::ios::trunc);
    if (!lhs.is_open() || !rhs.is_open()) {
        throw std::runtime_error("Failed to open temp files for integer benchmark");
    }
    lhs << header << '\n';
    rhs << header << '\n';
    for (std::size_t i = 0; i < scenario.rowCount; ++i) {
        const long long lhsValue = static_cast<long long>(i * 2);
        long long rhsValue = lhsValue;
        if (scenario.mismatchStride != 0 && (i % scenario.mismatchStride == 0)) {
            rhsValue = lhsValue - 1;
        }
        lhs << lhsValue << '\n';
        rhs << rhsValue << '\n';
    }
}

void WriteStringColumns(const PerfScenario& scenario, const TempFiles& files) {
    const std::string header = MakeHeader(ValueType::String);
    std::ofstream lhs(files.lhsPath.c_str(), std::ios::trunc);
    std::ofstream rhs(files.rhsPath.c_str(), std::ios::trunc);
    if (!lhs.is_open() || !rhs.is_open()) {
        throw std::runtime_error("Failed to open temp files for string benchmark");
    }
    lhs << header << '\n';
    rhs << header << '\n';
    for (std::size_t i = 0; i < scenario.rowCount; ++i) {
        std::string lhsValue = MakeStringValue(i, scenario.stringLength);
        std::string rhsValue = lhsValue;
        if (scenario.mismatchStride != 0 && (i % scenario.mismatchStride == 0)) {
            rhsValue[0] = (rhsValue[0] == 'Z') ? 'A' : static_cast<char>(rhsValue[0] + 1);
        }
        lhs << lhsValue << '\n';
        rhs << rhsValue << '\n';
    }
}

std::size_t FileSize(const std::string& path) {
    struct stat st;
    if (::stat(path.c_str(), &st) != 0) {
        throw std::runtime_error(fmt::format("Failed to stat {}: {}", path, std::strerror(errno)));
    }
    return static_cast<std::size_t>(st.st_size);
}

std::size_t CurrentRssBytes() {
#if defined(__APPLE__)
    mach_task_basic_info info;
    mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
    kern_return_t kr = task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &size);
    if (kr != KERN_SUCCESS) {
        return 0;
    }
    return static_cast<std::size_t>(info.resident_size);
#elif defined(__linux__)
    std::ifstream status("/proc/self/statm");
    if (!status.is_open()) {
        return 0;
    }
    long totalPages = 0;
    long rssPages = 0;
    status >> totalPages >> rssPages;
    long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0) {
        pageSize = 4096;
    }
    return static_cast<std::size_t>(rssPages) * static_cast<std::size_t>(pageSize);
#else
    return 0;
#endif
}

TempFiles PrepareScenarioFiles(const PerfScenario& scenario) {
    TempFiles files;
    files.lhsPath = MakeTempFile("diff_compare_lhs_");
    files.rhsPath = MakeTempFile("diff_compare_rhs_");
    files.outputPath = MakeTempFile("diff_compare_out_");

    switch (scenario.valueType) {
        case ValueType::Integer:
            WriteIntegerColumns(scenario, files);
            break;
        case ValueType::String:
            WriteStringColumns(scenario, files);
            break;
        default:
            throw std::invalid_argument(
                fmt::format("Unsupported ValueType scenario: {}", static_cast<int>(scenario.valueType)));
    }
    return files;
}

void TruncateFile(const std::string& path) {
    std::ofstream out(path.c_str(), std::ios::trunc);
}

void RegisterScenarioBenchmark(const PerfScenario& scenario) {
    auto benchmarkName = fmt::format("Diff/{}/{}Rows", scenario.name, scenario.rowCount);
    benchmark::RegisterBenchmark(benchmarkName.c_str(), [scenario](benchmark::State& state) {
        TempFiles files = PrepareScenarioFiles(scenario);

        const std::shared_ptr<const ColumnParser> parser = std::make_shared<SimpleColumnParser>();
        const std::shared_ptr<const ColumnInputProvider> inputProvider
            = std::make_shared<TxtColumnInputProvider>(parser);
        const std::shared_ptr<const SeriesComparatorFactory> comparatorFactory
            = std::make_shared<SeriesComparatorFactory>();
        const std::shared_ptr<const OutputFormatter> formatter = std::make_shared<PlainTextOutputFormatter>();
        const std::shared_ptr<const DiffOutputWriter> writer = std::make_shared<TxtDiffOutputWriter>(formatter);
        const ColumnProcessingPipeline pipeline(inputProvider, comparatorFactory, writer);

        const std::size_t lhsSize = FileSize(files.lhsPath);
        const std::size_t rhsSize = FileSize(files.rhsPath);
        std::size_t peakRssBytes = 0;

        for (auto _ : state) {
            state.PauseTiming();
            TruncateFile(files.outputPath);
            state.ResumeTiming();

            pipeline.run(files.lhsPath, files.rhsPath, files.outputPath);

            state.PauseTiming();
            const std::size_t currentRss = CurrentRssBytes();
            if (currentRss > peakRssBytes) {
                peakRssBytes = currentRss;
            }
            state.ResumeTiming();
        }

        const double iterations = static_cast<double>(state.iterations());
        state.SetItemsProcessed(static_cast<int64_t>(iterations * static_cast<double>(scenario.rowCount)));
        state.SetBytesProcessed(static_cast<int64_t>(iterations * static_cast<double>(lhsSize + rhsSize)));
        if (peakRssBytes > 0) {
            const double peakMb = static_cast<double>(peakRssBytes) / (1024.0 * 1024.0);
            const auto peakFlags = benchmark::Counter::kIsIterationInvariantRate;
            state.counters["PeakRSS_MB"] = benchmark::Counter(peakMb, peakFlags, benchmark::Counter::kIs1024);
        }
        const auto invariantFlags = benchmark::Counter::kIsIterationInvariant;
        state.counters["Rows"]
            = benchmark::Counter(static_cast<double>(scenario.rowCount), invariantFlags, benchmark::Counter::kIs1000);
        const double strideValue
            = static_cast<double>(scenario.mismatchStride == 0 ? scenario.rowCount + 1 : scenario.mismatchStride);
        state.counters["MismatchStride"] = benchmark::Counter(strideValue, invariantFlags, benchmark::Counter::kIs1000);
    })->UseRealTime();
}

void RegisterBenchmarks() {
    RegisterScenarioBenchmark(PerfScenario{"IntHeavy", ValueType::Integer, 1000000, 0, 10});
    RegisterScenarioBenchmark(PerfScenario{"IntIdentical", ValueType::Integer, 1000000, 0, 0});
    RegisterScenarioBenchmark(PerfScenario{"StrMedium", ValueType::String, 500000, 32, 8});
    RegisterScenarioBenchmark(PerfScenario{"StrLong", ValueType::String, 200000, 128, 5});
}

}  // namespace perf
}  // namespace diff_compare

int main(int argc, char** argv) {
    diff_compare::perf::RegisterBenchmarks();
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}
