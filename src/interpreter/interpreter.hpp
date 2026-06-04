#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ast/ast.hpp"

struct RuntimeError : std::runtime_error {
    Token token;
    RuntimeError(const Token& tok, const std::string& msg) : std::runtime_error(msg), token(tok) {
    }
};

struct Return : std::runtime_error {
    LoxLiteral value;
    Return(LoxLiteral val) : std::runtime_error(""), value(std::move(val)) {
    }
};

struct Function : Callable {
    Token name;
    std::vector<Token> params;
    std::vector<ast::Stmt> body;
    std::shared_ptr<Environment> closure;

    auto call(std::shared_ptr<Environment> env, const std::vector<LoxLiteral>& args, const Token& paren)
        -> LoxLiteral override;
    auto to_string() const -> std::string override;
};

class Environment {
  public:
    explicit Environment(std::shared_ptr<Environment> enclosing = nullptr);
    auto define(const std::string& name, LoxLiteral value) -> void;
    auto assign(const Token& name, LoxLiteral value) -> void;
    auto get(const Token& name) -> LoxLiteral;

  private:
    std::unordered_map<std::string, LoxLiteral> values_;
    std::shared_ptr<Environment> enclosing_;
};

auto evaluate(const ast::Expr& expr, std::shared_ptr<Environment> env) -> LoxLiteral;
auto interpret(const std::vector<ast::Stmt>& statements) -> void;
auto execute(const ast::Stmt& stmt, std::shared_ptr<Environment> env) -> void;

auto format_value(const LoxLiteral& value) -> std::string;
