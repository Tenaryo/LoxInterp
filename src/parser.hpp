#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "scanner.hpp"

namespace ast {

struct Binary;
struct Grouping;
struct Unary;

struct Literal {
    LoxLiteral value;
};

using Expr = std::variant<Literal, std::unique_ptr<Binary>, std::unique_ptr<Grouping>, std::unique_ptr<Unary>>;

struct Binary {
    Expr left;
    Token op;
    Expr right;
};

struct Grouping {
    Expr expression;
};

struct Unary {
    Token op;
    Expr right;
};

} // namespace ast

class Parser {
  public:
    explicit Parser(std::vector<Token> tokens);

    auto parse() -> ast::Expr;

  private:
    auto expression() -> ast::Expr;
    auto factor() -> ast::Expr;
    auto unary() -> ast::Expr;
    auto primary() -> ast::Expr;
    auto match(TokenType type) -> bool;
    auto check(TokenType type) -> bool;
    auto advance() -> Token;
    [[nodiscard]] auto is_at_end() const -> bool;
    auto peek() const -> Token;
    auto previous() -> Token;

    std::vector<Token> tokens_;
    std::size_t current_{0};
};

auto print_ast(const ast::Expr& expr) -> std::string;
