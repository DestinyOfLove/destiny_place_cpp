#include "diff_compare/ColumnProcessingPipeline.hpp"

#include <stdexcept>
#include <utility>

namespace diff_compare {

ColumnProcessingPipeline::ColumnProcessingPipeline(
    std::shared_ptr<const ColumnInputProvider> input_provider,
    std::shared_ptr<const ColumnComparatorFactory> comparator_factory,
    std::shared_ptr<const DiffOutputWriter> output_writer)
    : input_provider_(std::move(input_provider)),
      comparator_factory_(std::move(comparator_factory)),
      output_writer_(std::move(output_writer)) {
    if (!input_provider_) {
        throw std::invalid_argument("ColumnProcessingPipeline requires an input provider");
    }
    if (!comparator_factory_) {
        throw std::invalid_argument("ColumnProcessingPipeline requires a comparator factory");
    }
    if (!output_writer_) {
        throw std::invalid_argument("ColumnProcessingPipeline requires an output writer");
    }
}

void ColumnProcessingPipeline::run(const std::string& input_a,
                                   const std::string& input_b,
                                   const std::string& output_path) const {
    const ColumnData column_a = input_provider_->readColumn(input_a);
    const ColumnData column_b = input_provider_->readColumn(input_b);

    auto comparator = comparator_factory_->create(column_a.descriptor());
    const ColumnDiff diff = comparator->compare(column_a, column_b);
    output_writer_->write(diff, output_path);
}

}  // namespace diff_compare
