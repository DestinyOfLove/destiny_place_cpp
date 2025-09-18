#pragma once

#include <memory>
#include <string>

#include "diff_compare/OutputFormatter.hpp"

namespace diff_compare {

class DiffOutputWriter {
public:
    virtual ~DiffOutputWriter() = default;
    virtual void write(const ColumnDiff& diff, const std::string& path) const = 0;
};

class TxtDiffOutputWriter : public DiffOutputWriter {
public:
    explicit TxtDiffOutputWriter(std::shared_ptr<const OutputFormatter> formatter);

    void write(const ColumnDiff& diff, const std::string& path) const override;

private:
    std::shared_ptr<const OutputFormatter> formatter_;
};

}  // namespace diff_compare
