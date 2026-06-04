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

struct LoxClass;

struct Function : Callable {
    Token name;
    std::vector<Token> params;
    const std::vector<ast::Stmt>* body;
    std::shared_ptr<Environment> closure;
    std::weak_ptr<LoxClass> declaring_class_;
    bool is_init_{false};

    auto call(std::shared_ptr<Environment> env, const std::vector<LoxLiteral>& args, const Token& paren)
        -> LoxLiteral override;
    auto to_string() const -> std::string override;
};

struct LoxClass : Callable, std::enable_shared_from_this<LoxClass> {
    std::string name;
    std::shared_ptr<LoxClass> superclass_;
    std::unordered_map<std::string, std::shared_ptr<Function>> methods_;

    auto call(std::shared_ptr<Environment> env, const std::vector<LoxLiteral>& args, const Token& paren)
        -> LoxLiteral override;
    auto to_string() const -> std::string override;
    auto find_method(const std::string& method_name) -> std::shared_ptr<Function>;
};

struct LoxInstance : std::enable_shared_from_this<LoxInstance> {
    std::shared_ptr<LoxClass> klass;
    std::unordered_map<std::string, LoxLiteral> fields_;

    auto to_string() const -> std::string;
    auto get(const Token& name) -> LoxLiteral;
    auto set(const Token& name, LoxLiteral value) -> void;
};

class Environment {
  public:
    explicit Environment(std::shared_ptr<Environment> enclosing = nullptr);
    auto define(const std::string& name, LoxLiteral value) -> void;
    auto assign(const Token& name, LoxLiteral value) -> void;
    auto get(const Token& name) -> LoxLiteral;
    auto get_at(int depth, const std::string& name) -> LoxLiteral;
    auto assign_at(int depth, const std::string& name, LoxLiteral value) -> void;

  private:
    std::unordered_map<std::string, LoxLiteral> values_;
    std::shared_ptr<Environment> enclosing_;
};

auto evaluate(const ast::Expr& expr, std::shared_ptr<Environment> env) -> LoxLiteral;
auto interpret(const std::vector<ast::Stmt>& statements) -> void;
auto execute(const ast::Stmt& stmt, std::shared_ptr<Environment> env) -> void;

auto format_value(const LoxLiteral& value) -> std::string;
