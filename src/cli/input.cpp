#include "cli/input.hpp"

#include <charconv>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace pager::cli {
namespace {

template <typename UInt>
bool parse_unsigned(std::string_view text, UInt& value) {
    const auto end = text.data() + text.size();
    const auto result = std::from_chars(text.data(), end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

Policy parse_policy(std::string_view name) {
    if (name == "fifo") return Fifo{};
    if (name == "lru") return Lru{};
    if (name == "opt") return Opt{};
    if (name == "clock") return Clock{};
    if (name == "nru") return Nru{};
    if (name == "lfu") return Lfu{};
    if (name == "mfu") return Mfu{};
    throw std::invalid_argument("Algoritmo inválido: " + std::string(name));
}

}  // namespace

Options parse_arguments(int argc, const char* const argv[]) {
    Options options;
    bool has_policy = false;
    for (int i = 1; i < argc; ++i) {
        const std::string_view flag = argv[i];
        if (flag == "--verbose" || flag == "--visual") {
            if (options.mode != OutputMode::Summary) {
                throw std::invalid_argument("Use apenas uma ocorrência de --verbose ou --visual.");
            }
            options.mode = flag == "--verbose" ? OutputMode::Verbose : OutputMode::Visual;
            continue;
        }

        if (flag != "--algo" && flag != "--frames" && flag != "--trace") {
            throw std::invalid_argument("Argumento desconhecido: " + std::string(flag));
        }
        if (i + 1 == argc) {
            throw std::invalid_argument("Valor ausente para " + std::string(flag));
        }
        const std::string_view value = argv[++i];
        if (flag == "--algo" && !has_policy) {
            options.policy = parse_policy(value);
            has_policy = true;
        } else if (flag == "--frames" && options.frame_count == 0) {
            if (!parse_unsigned(value, options.frame_count) || options.frame_count == 0) {
                throw std::invalid_argument("--frames deve ser um inteiro maior ou igual a 1.");
            }
        } else if (flag == "--trace" && options.trace_path.empty()) {
            if (value.empty()) {
                throw std::invalid_argument("--trace exige um caminho de arquivo.");
            }
            options.trace_path = value;
        } else {
            throw std::invalid_argument("Argumento repetido: " + std::string(flag));
        }
    }
    if (!has_policy || options.frame_count == 0 || options.trace_path.empty()) {
        throw std::invalid_argument("--algo, --frames e --trace são obrigatórios.");
    }
    return options;
}

Trace read_trace(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Não foi possível abrir o traço: " + path);
    }

    Trace trace;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        std::istringstream fields(line);
        std::string page;
        if (!(fields >> page) || page.front() == '#') {
            continue;
        }
        Reference reference{};
        std::string access;
        std::string extra;
        const bool has_access = static_cast<bool>(fields >> access);
        const bool valid_access = !has_access || access == "R" || access == "W";
        if (!parse_unsigned(page, reference.page) || !valid_access || fields >> extra) {
            throw std::runtime_error(path + ": linha " + std::to_string(line_number) +
                                     ": referência inválida. Esperado: página [R|W].");
        }
        if (access == "W") {
            reference.type = AccessType::Write;
        }
        trace.push_back(reference);
    }
    if (input.bad() || !input.eof()) {
        throw std::runtime_error("Erro ao ler o traço: " + path);
    }
    return trace;
}

}  // namespace pager::cli
