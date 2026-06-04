#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.hpp"

class Resolver {
  public:
    Resolver();

    auto resolve(const std::vector<ast::Stmt>& statements) -> void;
    auto has_errors() const -> bool;

  private:
    auto resolve(const ast::Stmt& stmt) -> void;
    auto resolve(ast::Expr& expr) -> void;
    auto begin_scope() -> void;
    auto end_scope() -> void;
    auto declare(const Token& name) -> void;
    auto define(const Token& name) -> void;
    auto resolve_local(const ast::Expr& expr, const Token& name) -> void;
    auto error(const Token& token, std::string_view message) -> void;

    std::vector<std::unordered_map<std::string, bool>> scopes_;
    int function_depth_{0};
    int class_depth_{0};
    bool is_init_{false};
    bool had_error_{false};
};
