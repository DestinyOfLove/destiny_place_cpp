#pragma once

#include <memory>

#include "diff_compare/app/ColumnProcessingPipeline.hpp"

namespace diff_compare {

class ColumnDiffApp {
public:
    explicit ColumnDiffApp(std::shared_ptr<const ColumnProcessingPipeline> pipeline);

    int run(int argc, char* argv[]) const;

private:
    void validateArgs(int argc) const;
    void printUsage() const;

    std::shared_ptr<const ColumnProcessingPipeline> pipeline_;
};

}  // namespace diff_compare
