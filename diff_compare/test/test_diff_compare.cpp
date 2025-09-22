#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "diff_compare/app/ColumnProcessingPipeline.hpp"
#include "diff_compare/core/SeriesComparator.hpp"
#include "diff_compare/core/SeriesComparatorFactory.hpp"
#include "diff_compare/core/SeriesData.hpp"
#include "diff_compare/core/SeriesDescriptor.hpp"
#include "diff_compare/core/SeriesDiff.hpp"
#include "diff_compare/core/ValueType.hpp"
#include "diff_compare/io/ColumnInputProvider.hpp"
#include "diff_compare/io/ColumnParser.hpp"
#include "diff_compare/io/DiffOutputWriter.hpp"
#include "diff_compare/io/OutputFormatter.hpp"

namespace diff_compare {
namespace {

SeriesData makeSeriesData(std::string name, ValueType type, std::vector<std::string> values) {
    return SeriesData::fromStringValues(SeriesDescriptor(std::move(name), type), std::move(values));
}

std::vector<std::string> readFileLines(const std::string& path) {
    std::ifstream input(path.c_str());
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open file: " + path);
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string writeTempFile(const std::string& file_name, const std::string& contents) {
    const std::string path = ::testing::TempDir() + file_name;
    std::ofstream output(path.c_str(), std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to create temp file: " + path);
    }
    output << contents;
    return path;
}

}  // namespace

TEST(SeriesDescriptorTest, RejectsNameTypeMismatch) {
    EXPECT_THROW(SeriesDescriptor("Int_Invalid", ValueType::String), std::invalid_argument);
    EXPECT_THROW(SeriesDescriptor("Str_Invalid", ValueType::Integer), std::invalid_argument);
}

TEST(ValueTypeTest, ParsesKnownPrefixes) {
    EXPECT_EQ(ValueType::Integer, valueTypeFromHeader("Int_Score"));
    EXPECT_EQ(ValueType::String, valueTypeFromHeader("Str_Name"));
}

TEST(ValueTypeTest, AllowsRegisteringAdditionalPrefixes) {
    registerValueType("IntAlt_", ValueTraits{ValueType::Integer, "Integer"});
    EXPECT_EQ(ValueType::Integer, valueTypeFromHeader("IntAlt_Score"));
}

TEST(ValueTypeTest, RejectsDuplicatePrefixRegistration) {
    registerValueType("IntDup_", ValueTraits{ValueType::Integer, "Integer"});
    EXPECT_THROW(registerValueType("IntDup_", ValueTraits{ValueType::Integer, "Integer"}), std::invalid_argument);
}

TEST(ValueTypeTest, RejectsConflictingLabelRegistration) {
    EXPECT_THROW(registerValueType("IntConflict_", ValueTraits{ValueType::Integer, "Different"}),
                 std::invalid_argument);
}

TEST(ValueTypeTest, ThrowsOnUnsupportedPrefix) {
    EXPECT_THROW(valueTypeFromHeader("Foo"), std::invalid_argument);
}

TEST(ParallelNumericSeriesComparatorTest, ComputesNumericDifferences) {
    const SeriesData lhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"2", "4", "6"});
    const SeriesData rhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"1", "1", "2"});

    ParallelNumericSeriesComparator comparator;
    const SeriesDiff diff = comparator.compare(lhs, rhs);

    ASSERT_EQ(3u, diff.size());
    EXPECT_EQ("Int_Diff", diff.descriptor().name());
    EXPECT_EQ(ValueType::Integer, diff.descriptor().type());
    EXPECT_EQ("1", diff.valueAt(0));
    EXPECT_EQ("3", diff.valueAt(1));
    EXPECT_EQ("4", diff.valueAt(2));
}

TEST(ParallelNumericSeriesComparatorTest, RejectsTypeMismatch) {
    const SeriesData lhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"1"});
    const SeriesData rhs = makeSeriesData("Str_A", ValueType::String, std::vector<std::string>{"1"});

    ParallelNumericSeriesComparator comparator;
    EXPECT_THROW(comparator.compare(lhs, rhs), std::invalid_argument);
}

TEST(ParallelNumericSeriesComparatorTest, RejectsNameMismatch) {
    const SeriesData lhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"1"});
    const SeriesData rhs = makeSeriesData("Int_B", ValueType::Integer, std::vector<std::string>{"1"});

    ParallelNumericSeriesComparator comparator;
    EXPECT_THROW(comparator.compare(lhs, rhs), std::invalid_argument);
}

TEST(ParallelNumericSeriesComparatorTest, RejectsNonNumericValues) {
    const SeriesData lhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"10", "1x", "30"});
    const SeriesData rhs = makeSeriesData("Int_A", ValueType::Integer, std::vector<std::string>{"1", "2", "3"});

    ParallelNumericSeriesComparator comparator;
    EXPECT_THROW(comparator.compare(lhs, rhs), std::invalid_argument);
}

TEST(ParallelNumericSeriesComparatorTest, HandlesLargeInputs) {
    constexpr std::size_t kCount = 2048;
    std::vector<std::string> lhs_values;
    std::vector<std::string> rhs_values;
    lhs_values.reserve(kCount);
    rhs_values.reserve(kCount);
    for (std::size_t i = 0; i < kCount; ++i) {
        lhs_values.emplace_back(std::to_string(i * 2));
        rhs_values.emplace_back(std::to_string(i));
    }

    const SeriesData lhs = makeSeriesData("Int_Large", ValueType::Integer, lhs_values);
    const SeriesData rhs = makeSeriesData("Int_Large", ValueType::Integer, rhs_values);

    ParallelNumericSeriesComparator comparator;
    const SeriesDiff diff = comparator.compare(lhs, rhs);

    ASSERT_EQ(kCount, diff.size());
    EXPECT_EQ("Int_Diff", diff.descriptor().name());
    EXPECT_EQ("0", diff.valueAt(0));
    EXPECT_EQ(std::to_string((kCount - 1)), diff.valueAt(kCount - 1));
}

TEST(ParallelTextSeriesComparatorTest, MarksMatchesWithT) {
    const SeriesData lhs = makeSeriesData("Str_Name", ValueType::String, std::vector<std::string>{"Alice", "Bob"});
    const SeriesData rhs = makeSeriesData("Str_Name", ValueType::String, std::vector<std::string>{"Alice", "Charlie"});

    ParallelTextSeriesComparator comparator;
    const SeriesDiff diff = comparator.compare(lhs, rhs);

    EXPECT_EQ("Str_Diff", diff.descriptor().name());
    ASSERT_EQ(2u, diff.size());
    EXPECT_EQ("T", diff.valueAt(0));
    EXPECT_EQ("N", diff.valueAt(1));
}

TEST(SimpleColumnParserTest, TrimsWhitespaceAndSkipsEmptyLines) {
    std::istringstream input(" Str_Label \n value1 \n\n value2\r\n  \n");

    SimpleColumnParser parser;
    const SeriesData data = parser.parse(input);

    EXPECT_EQ("Str_Label", data.descriptor().name());
    EXPECT_EQ(ValueType::String, data.descriptor().type());
    ASSERT_EQ(2u, data.size());
    EXPECT_EQ("value1", data.valueAt(0));
    EXPECT_EQ("value2", data.valueAt(1));
}


TEST(ColumnInputProviderTest, RejectsEmptyFile) {
    const std::shared_ptr<const ColumnParser> parser = std::make_shared<SimpleColumnParser>();
    const TxtColumnInputProvider provider(parser);
    const std::string empty_path = writeTempFile("empty_column.txt", "");
    EXPECT_THROW(provider.readSeries(empty_path), std::invalid_argument);
}

TEST(ColumnProcessingPipelineTest, RunsEndToEndForIntegerColumns) {
    const std::string input_a = writeTempFile("input_a.txt", "Int_Value\n10\n20\n30\n");
    const std::string input_b = writeTempFile("input_b.txt", "Int_Value\n7\n15\n5\n");
    const std::string output_path = ::testing::TempDir() + std::string("diff_output.txt");

    const std::shared_ptr<const ColumnParser> parser = std::make_shared<SimpleColumnParser>();
    const std::shared_ptr<const ColumnInputProvider> input_provider = std::make_shared<TxtColumnInputProvider>(parser);
    const std::shared_ptr<const SeriesComparatorFactory> comparator_factory
        = std::make_shared<SeriesComparatorFactory>();
    const std::shared_ptr<const OutputFormatter> formatter = std::make_shared<PlainTextOutputFormatter>();
    const std::shared_ptr<const DiffOutputWriter> output_writer = std::make_shared<TxtDiffOutputWriter>(formatter);

    const ColumnProcessingPipeline pipeline(input_provider, comparator_factory, output_writer);
    pipeline.run(input_a, input_b, output_path);

    const std::vector<std::string> lines = readFileLines(output_path);
    ASSERT_EQ(4u, lines.size());
    EXPECT_EQ("Int_Diff", lines[0]);
    EXPECT_EQ("3", lines[1]);
    EXPECT_EQ("5", lines[2]);
    EXPECT_EQ("25", lines[3]);
}

}  // namespace diff_compare
