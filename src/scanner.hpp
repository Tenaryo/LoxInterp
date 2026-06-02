#pragma once

#include <string>
#include <variant>
#include <vector>

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    STAR,
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    EOF_,
};

using LoxLiteral = std::variant<std::monostate, std::string, double>;

struct Token {
    TokenType type;
    std::string lexeme;
    LoxLiteral literal;
    int line;
};

class Scanner {
  public:
    explicit Scanner(std::string source);

    auto scan_tokens() -> std::vector<Token>;
    auto has_errors() const -> bool;

  private:
    [[nodiscard]] auto is_at_end() const -> bool;
    auto advance() -> char;
    auto add_token(TokenType type) -> void;
    auto scan_token() -> void;
    auto error(int line, std::string_view message) -> void;
    auto match(char expected) -> bool;

    std::string source_;
    std::size_t start_{0};
    std::size_t current_{0};
    int line_{1};
    bool had_error_{false};
    std::vector<Token> tokens_;
};

auto format_token(const Token& token) -> std::string;
