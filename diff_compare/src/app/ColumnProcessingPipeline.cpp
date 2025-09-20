#include "diff_compare/app/ColumnProcessingPipeline.hpp"

#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

ColumnProcessingPipeline::ColumnProcessingPipeline(std::shared_ptr<const SeriesInputProvider> input_provider,
                                                   std::shared_ptr<const SeriesComparatorFactory> comparator_factory,
                                                   std::shared_ptr<const SeriesOutputWriter> output_writer)
    : input_provider_(std::move(input_provider)),
      comparator_factory_(std::move(comparator_factory)),
      output_writer_(std::move(output_writer)) {
    if (!input_provider_) {
        throw std::invalid_argument(fmt::format("ColumnProcessingPipeline requires an input provider"));
    }
    if (!comparator_factory_) {
        throw std::invalid_argument(fmt::format("ColumnProcessingPipeline requires a comparator factory"));
    }
    if (!output_writer_) {
        throw std::invalid_argument(fmt::format("ColumnProcessingPipeline requires an output writer"));
    }
}

void ColumnProcessingPipeline::run(const std::string& input_a,
                                   const std::string& input_b,
                                   const std::string& output_path) const {
    const SeriesData series_a = input_provider_->readSeries(input_a);
    const SeriesData series_b = input_provider_->readSeries(input_b);

    auto comparator = comparator_factory_->create(series_a.descriptor());
    const SeriesDiff diff = comparator->compare(series_a, series_b);
    output_writer_->writeSeries(diff, output_path);
}

}  // namespace diff_compare
