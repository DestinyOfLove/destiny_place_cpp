#include <memory>

#include "diff_compare/ColumnDiffApp.hpp"
#include "diff_compare/ColumnInputProvider.hpp"
#include "diff_compare/ColumnParser.hpp"
#include "diff_compare/ColumnProcessingPipeline.hpp"
#include "diff_compare/DiffOutputWriter.hpp"
#include "diff_compare/OutputFormatter.hpp"
#include "diff_compare/SeriesComparatorFactory.hpp"

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
