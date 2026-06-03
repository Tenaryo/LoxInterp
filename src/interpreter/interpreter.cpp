#include "interpreter/interpreter.hpp"

#include <cmath>
#include <iostream>

#include "scanner/scanner.hpp"
#include "util/overloaded.hpp"

RuntimeError::RuntimeError(const Token& tok, const std::string& msg) : std::runtime_error(msg), token(tok) {
}

auto Environment::define(const std::string& name, LoxLiteral value) -> void {
    values_[name] = std::move(value);
}

auto Environment::get(const Token& name) -> LoxLiteral {
    auto it = values_.find(name.lexeme);
    if (it != values_.end()) {
        return it->second;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

auto evaluate(const ast::Expr& expr, Environment& env) -> LoxLiteral {
    return std::visit(
        overloaded{
            [](const ast::Literal& lit) -> LoxLiteral { return lit.value; },
            [&](const std::unique_ptr<ast::Grouping>& grp) -> LoxLiteral { return evaluate(grp->expression, env); },
            [&](const std::unique_ptr<ast::Unary>& un) -> LoxLiteral {
                auto right = evaluate(un->right, env);
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
            [&](const std::unique_ptr<ast::Binary>& bin) -> LoxLiteral {
                auto left = evaluate(bin->left, env);
                auto right = evaluate(bin->right, env);
                if (bin->op.type == TokenType::BANG_EQUAL || bin->op.type == TokenType::EQUAL_EQUAL) {
                    bool is_equal = left == right;
                    if (bin->op.type == TokenType::BANG_EQUAL) {
                        return !is_equal;
                    }
                    return is_equal;
                }
                if (bin->op.type == TokenType::PLUS) {
                    const auto* lstr = std::get_if<std::string>(&left);
                    const auto* rstr = std::get_if<std::string>(&right);
                    if (lstr != nullptr && rstr != nullptr) {
                        return *lstr + *rstr;
                    }
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
            [&](const std::unique_ptr<ast::Variable>& var) -> LoxLiteral { return env.get(var->name); },
        },
        expr);
}

auto interpret(const std::vector<ast::Stmt>& statements) -> void {
    Environment env;
    for (const auto& stmt : statements) {
        execute(stmt, env);
    }
}

auto execute(const ast::Stmt& stmt, Environment& env) -> void {
    std::visit(overloaded{
                   [&](const std::unique_ptr<ast::PrintStmt>& print) {
                       auto value = evaluate(print->expression, env);
                       std::cout << format_value(value) << '\n';
                   },
                   [&](const std::unique_ptr<ast::ExprStmt>& expr_stmt) { evaluate(expr_stmt->expression, env); },
                   [&](const std::unique_ptr<ast::VarStmt>& var_stmt) {
                       auto value = evaluate(var_stmt->initializer, env);
                       env.define(var_stmt->name.lexeme, std::move(value));
                   },
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
