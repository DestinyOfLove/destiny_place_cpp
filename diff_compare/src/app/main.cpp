#include <memory>

#include "diff_compare/app/ColumnDiffApp.hpp"
#include "diff_compare/app/ColumnProcessingPipeline.hpp"
#include "diff_compare/core/SeriesComparatorFactory.hpp"
#include "diff_compare/io/ColumnInputProvider.hpp"
#include "diff_compare/io/ColumnParser.hpp"
#include "diff_compare/io/DiffOutputWriter.hpp"
#include "diff_compare/io/OutputFormatter.hpp"

int main(int argc, char* argv[]) {
    using namespace diff_compare;

    auto parser = std::make_shared<SimpleColumnParser>();
    auto input_provider = std::make_shared<TxtColumnInputProvider>(parser);
    auto comparator_factory = std::make_shared<SeriesComparatorFactory>();
    auto formatter = std::make_shared<PlainTextOutputFormatter>();
    auto output_writer = std::make_shared<TxtDiffOutputWriter>(formatter);
    auto pipeline = std::make_shared<ColumnProcessingPipeline>(input_provider, comparator_factory, output_writer);

    ColumnDiffApp app(pipeline);
    return app.run(argc, argv);
}
