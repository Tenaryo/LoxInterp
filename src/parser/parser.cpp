#include "parser/parser.hpp"

#include <cmath>
#include <iostream>

#include "util/overloaded.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
}

auto Parser::parse() -> ast::Expr {
    try {
        return expression();
    } catch (const ParseError&) {
        return ast::Literal{std::monostate{}};
    }
}

auto Parser::parse_statements() -> std::vector<ast::Stmt> {
    std::vector<ast::Stmt> statements;
    while (!is_at_end()) {
        try {
            statements.push_back(statement());
        } catch (const ParseError&) {
            synchronize();
        }
    }
    return statements;
}

auto Parser::statement() -> ast::Stmt {
    if (match(TokenType::PRINT)) {
        return print_statement();
    }
    if (match(TokenType::VAR)) {
        return var_declaration();
    }
    if (match(TokenType::LEFT_BRACE)) {
        return block();
    }
    if (match(TokenType::IF)) {
        return if_statement();
    }
    return expr_statement();
}

auto Parser::print_statement() -> ast::Stmt {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ast::PrintStmt>(std::move(expr));
}

auto Parser::expr_statement() -> ast::Stmt {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ast::ExprStmt>(std::move(expr));
}

auto Parser::var_declaration() -> ast::Stmt {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    ast::Expr initializer = ast::Literal{std::monostate{}};
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<ast::VarStmt>(name, std::move(initializer));
}

auto Parser::block() -> ast::Stmt {
    std::vector<ast::Stmt> statements;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(statement());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<ast::BlockStmt>(std::move(statements));
}

auto Parser::if_statement() -> ast::Stmt {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto then_branch = statement();
    std::optional<ast::Stmt> else_branch;
    if (match(TokenType::ELSE)) {
        else_branch = statement();
    }
    return std::make_unique<ast::IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

auto Parser::has_errors() const -> bool {
    return had_error_;
}

auto Parser::expression() -> ast::Expr {
    return assignment();
}

auto Parser::assignment() -> ast::Expr {
    auto expr = logic_or();
    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();
        if (auto* var = std::get_if<std::unique_ptr<ast::Variable>>(&expr)) {
            return std::make_unique<ast::Assign>((*var)->name, std::move(value));
        }
        error(equals, "Invalid assignment target.");
    }
    return expr;
}

auto Parser::logic_or() -> ast::Expr {
    auto expr = logic_and();
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = logic_and();
        expr = std::make_unique<ast::Logical>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::logic_and() -> ast::Expr {
    return equality();
}

auto Parser::equality() -> ast::Expr {
    auto expr = comparison();
    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::comparison() -> ast::Expr {
    auto expr = term();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS)
           || match(TokenType::LESS_EQUAL)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::term() -> ast::Expr {
    auto expr = factor();
    while (match(TokenType::MINUS) || match(TokenType::PLUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::factor() -> ast::Expr {
    auto expr = unary();
    while (match(TokenType::SLASH) || match(TokenType::STAR)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::unary() -> ast::Expr {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<ast::Unary>(op, std::move(right));
    }
    return primary();
}

auto Parser::primary() -> ast::Expr {
    if (match(TokenType::FALSE_)) {
        return ast::Literal{false};
    }
    if (match(TokenType::TRUE_)) {
        return ast::Literal{true};
    }
    if (match(TokenType::NIL)) {
        return ast::Literal{std::monostate{}};
    }
    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
        return ast::Literal{previous().literal};
    }
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<ast::Variable>(previous());
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<ast::Grouping>(std::move(expr));
    }
    throw error(peek(), "Expect expression.");
}

auto Parser::match(TokenType type) -> bool {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

auto Parser::check(TokenType type) -> bool {
    if (is_at_end()) {
        return false;
    }
    return peek().type == type;
}

auto Parser::advance() -> Token {
    if (!is_at_end()) {
        current_++;
    }
    return previous();
}

auto Parser::is_at_end() const -> bool {
    return peek().type == TokenType::EOF_;
}

auto Parser::peek() const -> Token {
    return tokens_[current_];
}

auto Parser::previous() -> Token {
    return tokens_[current_ - 1];
}

auto Parser::error(const Token& token, std::string_view message) -> ParseError {
    had_error_ = true;
    if (token.type == TokenType::EOF_) {
        std::cerr << "[line " << token.line << "] Error at end: " << message << '\n';
    } else {
        std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << '\n';
    }
    return ParseError{};
}

auto Parser::consume(TokenType type, std::string_view message) -> Token {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

auto Parser::synchronize() -> void {
    if (!is_at_end()) {
        current_++;
    }

    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) {
            return;
        }
        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            break;
        }
        current_++;
    }
}

auto print_ast(const ast::Expr& expr) -> std::string {
    auto format_literal = [](const LoxLiteral& literal) -> std::string {
        if (std::holds_alternative<std::monostate>(literal)) {
            return "nil";
        }
        if (const auto* b = std::get_if<bool>(&literal)) {
            return *b ? "true" : "false";
        }
        if (const auto* val = std::get_if<double>(&literal)) {
            double v = *val;
            if (v == std::floor(v) && !std::isinf(v)) {
                return std::to_string(static_cast<long long>(v)) + ".0";
            }
            std::string s = std::to_string(v);
            s.erase(s.find_last_not_of('0') + 1);
            if (s.back() == '.') {
                s += '0';
            }
            return s;
        }
        if (const auto* str = std::get_if<std::string>(&literal)) {
            return *str;
        }
        return "nil";
    };

    return std::visit(overloaded{
                          [&](const ast::Literal& lit) -> std::string { return format_literal(lit.value); },
                          [&](const std::unique_ptr<ast::Grouping>& grp) -> std::string {
                              return "(group " + print_ast(grp->expression) + ")";
                          },
                          [&](const std::unique_ptr<ast::Unary>& un) -> std::string {
                              return "(" + un->op.lexeme + " " + print_ast(un->right) + ")";
                          },
                          [&](const std::unique_ptr<ast::Binary>& bin) -> std::string {
                              return "(" + bin->op.lexeme + " " + print_ast(bin->left) + " " + print_ast(bin->right)
                                     + ")";
                          },
                          [&](const std::unique_ptr<ast::Variable>& var) -> std::string { return var->name.lexeme; },
                          [&](const std::unique_ptr<ast::Assign>& assign) -> std::string {
                              return assign->name.lexeme + " = " + print_ast(assign->value);
                          },
                          [&](const std::unique_ptr<ast::Logical>& logical) -> std::string {
                              return "(" + print_ast(logical->left) + " " + logical->op.lexeme + " "
                                     + print_ast(logical->right) + ")";
                          },
                      },
                      expr);
}
