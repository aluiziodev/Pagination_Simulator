#ifndef PAGER_CLI_INPUT_HPP
#define PAGER_CLI_INPUT_HPP

#include "core/contracts.hpp"
#include "policies/policies.hpp"

#include <string>

namespace pager::cli {

enum class OutputMode { Summary, Verbose, Visual };

struct Options {
    Policy policy;
    std::size_t frame_count = 0;
    std::string trace_path;
    OutputMode mode = OutputMode::Summary;
};

Options parse_arguments(int argc, const char* const argv[]);
Trace read_trace(const std::string& path);

}  // namespace pager::cli

#endif
