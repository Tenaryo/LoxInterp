#include "parser.hpp"

#include <cmath>

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
}

auto Parser::parse() -> ast::Expr {
    return expression();
}

auto Parser::expression() -> ast::Expr {
    return factor();
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
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        match(TokenType::RIGHT_PAREN);
        return std::make_unique<ast::Grouping>(std::move(expr));
    }
    return ast::Literal{std::monostate{}};
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

auto print_ast(const ast::Expr& expr) -> std::string {
    return std::visit(
        [](const auto& node) -> std::string {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, ast::Literal>) {
                if (std::holds_alternative<std::monostate>(node.value)) {
                    return "nil";
                }
                if (const auto* b = std::get_if<bool>(&node.value)) {
                    return *b ? "true" : "false";
                }
                if (const auto* val = std::get_if<double>(&node.value)) {
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
                if (const auto* str = std::get_if<std::string>(&node.value)) {
                    return *str;
                }
                return "nil";
            }
            if constexpr (std::is_same_v<T, std::unique_ptr<ast::Grouping>>) {
                return "(group " + print_ast(node->expression) + ")";
            }
            if constexpr (std::is_same_v<T, std::unique_ptr<ast::Unary>>) {
                return "(" + node->op.lexeme + " " + print_ast(node->right) + ")";
            }
            if constexpr (std::is_same_v<T, std::unique_ptr<ast::Binary>>) {
                return "(" + node->op.lexeme + " " + print_ast(node->left) + " " + print_ast(node->right) + ")";
            }
            return "";
        },
        expr);
}
