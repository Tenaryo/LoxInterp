#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "parser/parser.hpp"

struct RuntimeError : std::runtime_error {
    Token token;
    RuntimeError(const Token& tok, const std::string& msg);
};

class Environment {
  public:
    auto define(const std::string& name, LoxLiteral value) -> void;
    auto get(const Token& name) -> LoxLiteral;

  private:
    std::unordered_map<std::string, LoxLiteral> values_;
};

auto evaluate(const ast::Expr& expr, Environment& env) -> LoxLiteral;
auto interpret(const std::vector<ast::Stmt>& statements) -> void;
auto execute(const ast::Stmt& stmt, Environment& env) -> void;

auto format_value(const LoxLiteral& value) -> std::string;
