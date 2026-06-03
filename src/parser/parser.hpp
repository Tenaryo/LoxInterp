#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "scanner/scanner.hpp"

namespace ast {

struct Binary;
struct Grouping;
struct Unary;
struct Variable;

struct Literal {
    LoxLiteral value;
};

using Expr = std::variant<Literal,
                          std::unique_ptr<Binary>,
                          std::unique_ptr<Grouping>,
                          std::unique_ptr<Unary>,
                          std::unique_ptr<Variable>>;

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

struct PrintStmt;
struct ExprStmt;
struct VarStmt;

using Stmt = std::variant<std::unique_ptr<PrintStmt>, std::unique_ptr<ExprStmt>, std::unique_ptr<VarStmt>>;

struct PrintStmt {
    Expr expression;
};

struct ExprStmt {
    Expr expression;
};

struct VarStmt {
    Token name;
    Expr initializer;
};

struct Variable {
    Token name;
};

} // namespace ast

class Parser {
  public:
    explicit Parser(std::vector<Token> tokens);

    auto parse() -> ast::Expr;
    auto parse_statements() -> std::vector<ast::Stmt>;
    auto has_errors() const -> bool;

  private:
    struct ParseError : std::runtime_error {
        ParseError() : std::runtime_error("") {
        }
    };
    auto statement() -> ast::Stmt;
    auto print_statement() -> ast::Stmt;
    auto expr_statement() -> ast::Stmt;
    auto var_declaration() -> ast::Stmt;
    auto expression() -> ast::Expr;
    auto equality() -> ast::Expr;
    auto comparison() -> ast::Expr;
    auto term() -> ast::Expr;
    auto factor() -> ast::Expr;
    auto unary() -> ast::Expr;
    auto primary() -> ast::Expr;
    auto match(TokenType type) -> bool;
    auto check(TokenType type) -> bool;
    auto advance() -> Token;
    [[nodiscard]] auto is_at_end() const -> bool;
    auto peek() const -> Token;
    auto previous() -> Token;
    auto error(const Token& token, std::string_view message) -> ParseError;
    auto consume(TokenType type, std::string_view message) -> Token;
    auto synchronize() -> void;

    std::vector<Token> tokens_;
    std::size_t current_{0};
    bool had_error_{false};
};

auto print_ast(const ast::Expr& expr) -> std::string;
