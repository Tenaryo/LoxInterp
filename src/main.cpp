#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

[[nodiscard]] auto read_file_contents(const std::string& filename) -> std::string {
    std::ifstream file(filename);
    if (!file.is_open()) [[unlikely]] {
        std::cerr << "Error reading file: " << filename << '\n';
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

auto main(int argc, char* argv[]) -> int {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 3) [[unlikely]] {
        std::cerr << "Usage: ./your_program tokenize <filename>" << '\n';
        return 1;
    }

    const std::string command = argv[1];

    if (command == "tokenize") {
        std::string file_contents = read_file_contents(argv[2]);

        if (!file_contents.empty()) {
            std::cerr << "Scanner not implemented" << '\n';
            return 1;
        }
        std::cout << "EOF  null" << '\n';

    } else {
        std::cerr << "Unknown command: " << command << '\n';
        return 1;
    }

    return 0;
}
