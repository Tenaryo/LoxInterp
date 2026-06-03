#include "interpreter/interpreter.hpp"

#include <cmath>
#include <iostream>

#include "scanner/scanner.hpp"
#include "util/overloaded.hpp"

RuntimeError::RuntimeError(const Token& tok, const std::string& msg) : std::runtime_error(msg), token(tok) {
}

auto evaluate(const ast::Expr& expr) -> LoxLiteral {
    return std::visit(
        overloaded{
            [](const ast::Literal& lit) -> LoxLiteral { return lit.value; },
            [](const std::unique_ptr<ast::Grouping>& grp) -> LoxLiteral { return evaluate(grp->expression); },
            [](const std::unique_ptr<ast::Unary>& un) -> LoxLiteral {
                auto right = evaluate(un->right);
                if (un->op.type == TokenType::BANG) {
                    bool is_falsy = std::holds_alternative<std::monostate>(right)
                                    || (std::holds_alternative<bool>(right) && !std::get<bool>(right));
                    return is_falsy;
                }
                if (un->op.type == TokenType::MINUS) {
                    if (const auto* val = std::get_if<double>(&right)) {
                        return -*val;
                    }
                    throw RuntimeError(un->op, "Operand must be a number.");
                }
                return std::monostate{};
            },
            [](const std::unique_ptr<ast::Binary>& bin) -> LoxLiteral {
                auto left = evaluate(bin->left);
                auto right = evaluate(bin->right);
                if (bin->op.type == TokenType::PLUS) {
                    const auto* lstr = std::get_if<std::string>(&left);
                    const auto* rstr = std::get_if<std::string>(&right);
                    if (lstr != nullptr && rstr != nullptr) {
                        return *lstr + *rstr;
                    }
                }
                if (bin->op.type == TokenType::BANG_EQUAL || bin->op.type == TokenType::EQUAL_EQUAL) {
                    bool is_equal = left == right;
                    if (bin->op.type == TokenType::BANG_EQUAL) {
                        return !is_equal;
                    }
                    return is_equal;
                }
                const auto* lhs = std::get_if<double>(&left);
                const auto* rhs = std::get_if<double>(&right);
                if (lhs == nullptr || rhs == nullptr) {
                    throw RuntimeError(bin->op, "Operands must be numbers.");
                }
                if (bin->op.type == TokenType::STAR) {
                    return *lhs * *rhs;
                }
                if (bin->op.type == TokenType::SLASH) {
                    return *lhs / *rhs;
                }
                if (bin->op.type == TokenType::PLUS) {
                    return *lhs + *rhs;
                }
                if (bin->op.type == TokenType::MINUS) {
                    return *lhs - *rhs;
                }
                if (bin->op.type == TokenType::GREATER) {
                    return *lhs > *rhs;
                }
                if (bin->op.type == TokenType::GREATER_EQUAL) {
                    return *lhs >= *rhs;
                }
                if (bin->op.type == TokenType::LESS) {
                    return *lhs < *rhs;
                }
                if (bin->op.type == TokenType::LESS_EQUAL) {
                    return *lhs <= *rhs;
                }
                return std::monostate{};
            },
            [](const auto&) -> LoxLiteral { return std::monostate{}; },
        },
        expr);
}

auto interpret(const std::vector<ast::Stmt>& statements) -> void {
    for (const auto& stmt : statements) {
        execute(stmt);
    }
}

auto execute(const ast::Stmt& stmt) -> void {
    std::visit(overloaded{
                   [](const std::unique_ptr<ast::PrintStmt>& print) {
                       auto value = evaluate(print->expression);
                       std::cout << format_value(value) << '\n';
                   },
                   [](const std::unique_ptr<ast::ExprStmt>& expr_stmt) { evaluate(expr_stmt->expression); },
               },
               stmt);
}

auto format_value(const LoxLiteral& value) -> std::string {
    if (std::holds_alternative<std::monostate>(value)) {
        return "nil";
    }
    if (const auto* b = std::get_if<bool>(&value)) {
        return *b ? "true" : "false";
    }
    if (const auto* val = std::get_if<double>(&value)) {
        double v = *val;
        if (v == std::floor(v) && !std::isinf(v)) {
            return std::to_string(static_cast<long long>(v));
        }
        std::string s = std::to_string(v);
        s.erase(s.find_last_not_of('0') + 1);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
    }
    if (const auto* str = std::get_if<std::string>(&value)) {
        return *str;
    }
    return "nil";
}
