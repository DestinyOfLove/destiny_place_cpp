#include "diff_compare/app/ColumnDiffApp.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <fmt/core.h>

namespace diff_compare {

ColumnDiffApp::ColumnDiffApp(std::shared_ptr<const ColumnProcessingPipeline> pipeline)
    : pipeline_(std::move(pipeline)) {
    if (!pipeline_) {
        throw std::invalid_argument(fmt::format("ColumnDiffApp requires a pipeline"));
    }
}

int ColumnDiffApp::run(int argc, char* argv[]) const {
    try {
        validateArgs(argc);
        pipeline_->run(argv[1], argv[2], argv[3]);
        return 0;
    } catch (const std::invalid_argument& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        printUsage();
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 2;
    }
}

void ColumnDiffApp::validateArgs(int argc) const {
    if (argc != 4) {
        throw std::invalid_argument(fmt::format("Expected exactly three arguments: <inputA> <inputB> <output>"));
    }
}

void ColumnDiffApp::printUsage() const {
    std::cerr << "Usage: diff_compare <inputA> <inputB> <output>" << '\n';
}

}  // namespace diff_compare
