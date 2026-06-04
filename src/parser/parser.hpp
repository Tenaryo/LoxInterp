#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast/ast.hpp"
#include "scanner/scanner.hpp"

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
    auto declaration() -> ast::Stmt;
    auto print_statement() -> ast::Stmt;
    auto expr_statement() -> ast::Stmt;
    auto var_declaration() -> ast::Stmt;
    auto function_declaration() -> ast::Stmt;
    auto class_declaration() -> ast::Stmt;
    auto block() -> ast::Stmt;
    auto if_statement() -> ast::Stmt;
    auto while_statement() -> ast::Stmt;
    auto for_statement() -> ast::Stmt;
    auto return_statement() -> ast::Stmt;
    auto expression() -> ast::Expr;
    auto assignment() -> ast::Expr;
    auto logic_or() -> ast::Expr;
    auto logic_and() -> ast::Expr;
    auto equality() -> ast::Expr;
    auto comparison() -> ast::Expr;
    auto term() -> ast::Expr;
    auto factor() -> ast::Expr;
    auto unary() -> ast::Expr;
    auto call() -> ast::Expr;
    auto finish_call(ast::Expr callee) -> ast::Expr;
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
