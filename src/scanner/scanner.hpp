#pragma once

#include <memory>
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
    SLASH,
    STRING,
    NUMBER,
    IDENTIFIER,
    AND,
    CLASS,
    ELSE,
    FALSE_,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE_,
    VAR,
    WHILE,
    EOF_,
};

class Environment;
struct Callable;
struct LoxInstance;

using TokenLiteral = std::variant<std::monostate, bool, std::string, double>;
using LoxLiteral
    = std::variant<std::monostate, bool, std::string, double, std::shared_ptr<Callable>, std::shared_ptr<LoxInstance>>;

inline auto to_lox_literal(const TokenLiteral& lit) -> LoxLiteral {
    if (std::holds_alternative<std::monostate>(lit)) {
        return std::monostate{};
    }
    if (std::holds_alternative<bool>(lit)) {
        return std::get<bool>(lit);
    }
    if (std::holds_alternative<std::string>(lit)) {
        return std::get<std::string>(lit);
    }
    return std::get<double>(lit);
}

struct Token {
    TokenType type;
    std::string lexeme;
    TokenLiteral literal;
    int line;
};
struct Callable {
    virtual auto call(std::shared_ptr<Environment> env, const std::vector<LoxLiteral>& args, const Token& paren)
        -> LoxLiteral
        = 0;
    virtual ~Callable() = default;
    virtual auto to_string() const -> std::string = 0;
};

class Scanner {
  public:
    explicit Scanner(std::string source);

    auto scan_tokens() -> std::vector<Token>;
    auto has_errors() const -> bool;

  private:
    [[nodiscard]] auto is_at_end() const -> bool;
    auto advance() -> char;
    auto peek() const -> char;
    auto peek_next() const -> char;
    auto add_token(TokenType type) -> void;
    auto add_token(TokenType type, TokenLiteral literal) -> void;
    auto scan_token() -> void;
    auto error(int line, std::string_view message) -> void;
    auto match(char expected) -> bool;
    auto is_digit(char ch) const -> bool;
    auto is_alpha(char ch) const -> bool;
    auto is_alphanumeric(char ch) const -> bool;

    std::string source_;
    std::size_t start_{0};
    std::size_t current_{0};
    int line_{1};
    bool had_error_{false};
    std::vector<Token> tokens_;
};

auto format_token(const Token& token) -> std::string;
