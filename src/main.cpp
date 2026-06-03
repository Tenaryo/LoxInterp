#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "parser.hpp"
#include "scanner.hpp"

auto read_file_contents(const std::filesystem::path& path) -> std::string {
    std::ifstream file(path);
    if (!file.is_open()) [[unlikely]] {
        std::cerr << "Error reading file: " << path << '\n';
        std::exit(EXIT_FAILURE);
    }
    file.seekg(0, std::ios::end);
    std::string content(static_cast<std::size_t>(file.tellg()), '\0');
    file.seekg(0);
    file.read(content.data(), static_cast<std::streamsize>(content.size()));
    return content;
}

auto main(int argc, char* argv[]) -> int {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << '\n';
        return EXIT_FAILURE;
    }

    constexpr int kLexicalErrorExit = 65;
    const std::string_view command = argv[1];

    if (command == "tokenize") {
        std::string file_contents = read_file_contents(argv[2]);
        Scanner scanner(std::move(file_contents));
        auto tokens = scanner.scan_tokens();
        for (const auto& token : tokens) {
            std::cout << format_token(token) << '\n';
        }
        if (scanner.has_errors()) {
            return kLexicalErrorExit;
        }
    } else if (command == "parse") {
        std::string file_contents = read_file_contents(argv[2]);
        Scanner scanner(std::move(file_contents));
        auto tokens = scanner.scan_tokens();
        if (scanner.has_errors()) {
            return kLexicalErrorExit;
        }
        Parser parser(std::move(tokens));
        auto expr = parser.parse();
        if (parser.has_errors()) {
            return kLexicalErrorExit;
        }
        std::cout << print_ast(expr) << '\n';
    } else {
        std::cerr << "Unknown command: " << command << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
