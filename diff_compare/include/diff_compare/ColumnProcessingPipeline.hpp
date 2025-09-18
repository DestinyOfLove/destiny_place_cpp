#pragma once

#include <memory>
#include <string>

#include "diff_compare/ColumnComparatorFactory.hpp"
#include "diff_compare/ColumnInputProvider.hpp"
#include "diff_compare/DiffOutputWriter.hpp"

namespace diff_compare {

class ColumnProcessingPipeline {
public:
    ColumnProcessingPipeline(std::shared_ptr<const ColumnInputProvider> input_provider,
                             std::shared_ptr<const ColumnComparatorFactory> comparator_factory,
                             std::shared_ptr<const DiffOutputWriter> output_writer);

    void run(const std::string& input_a,
             const std::string& input_b,
             const std::string& output_path) const;

private:
    std::shared_ptr<const ColumnInputProvider> input_provider_;
    std::shared_ptr<const ColumnComparatorFactory> comparator_factory_;
    std::shared_ptr<const DiffOutputWriter> output_writer_;
};

}  // namespace diff_compare
