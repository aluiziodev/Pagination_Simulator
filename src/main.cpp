#include "cli/input.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        const auto options = pager::cli::parse_arguments(argc, argv);
        const auto trace = pager::cli::read_trace(options.trace_path);
    } catch (const std::invalid_argument& error) {
        std::cerr << error.what() << '\n'
                  << "Uso: " << argv[0]
                  << " --algo <ALGO> --frames <N> --trace <arquivo> [--verbose | --visual]\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
