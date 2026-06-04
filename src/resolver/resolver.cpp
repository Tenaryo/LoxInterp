#include "resolver/resolver.hpp"

#include <iostream>

#include "util/overloaded.hpp"

Resolver::Resolver() {
}

auto Resolver::resolve(const std::vector<ast::Stmt>& statements) -> void {
    begin_scope();
    for (const auto& stmt : statements) {
        resolve(stmt);
    }
    end_scope();
}

auto Resolver::resolve(const ast::Stmt& stmt) -> void {
    std::visit(
        overloaded{
            [&](const std::unique_ptr<ast::PrintStmt>& print) { resolve(const_cast<ast::Expr&>(print->expression)); },
            [&](const std::unique_ptr<ast::ExprStmt>& expr_stmt) {
                resolve(const_cast<ast::Expr&>(expr_stmt->expression));
            },
            [&](const std::unique_ptr<ast::VarStmt>& var_stmt) {
                declare(var_stmt->name);
                resolve(const_cast<ast::Expr&>(var_stmt->initializer));
                define(var_stmt->name);
            },
            [&](const std::unique_ptr<ast::BlockStmt>& block) {
                begin_scope();
                for (const auto& s : block->statements) {
                    resolve(s);
                }
                end_scope();
            },
            [&](const std::unique_ptr<ast::IfStmt>& if_stmt) {
                resolve(const_cast<ast::Expr&>(if_stmt->condition));
                resolve(if_stmt->then_branch);
                if (if_stmt->else_branch.has_value()) {
                    resolve(*if_stmt->else_branch);
                }
            },
            [&](const std::unique_ptr<ast::WhileStmt>& while_stmt) {
                resolve(const_cast<ast::Expr&>(while_stmt->condition));
                resolve(while_stmt->body);
            },
            [&](const std::unique_ptr<ast::FunctionStmt>& func) {
                declare(func->name);
                define(func->name);
                begin_scope();
                for (const auto& param : func->params) {
                    declare(param);
                    define(param);
                }
                for (const auto& s : func->body) {
                    resolve(s);
                }
                end_scope();
            },
            [&](const std::unique_ptr<ast::ReturnStmt>& ret) {
                if (ret->value.has_value()) {
                    resolve(*ret->value);
                }
            },
        },
        stmt);
}

auto Resolver::resolve(ast::Expr& expr) -> void {
    std::visit(overloaded{
                   [&](const ast::Literal& /*lit*/) {},
                   [&](const std::unique_ptr<ast::Grouping>& grp) { resolve(const_cast<ast::Expr&>(grp->expression)); },
                   [&](const std::unique_ptr<ast::Unary>& un) { resolve(const_cast<ast::Expr&>(un->right)); },
                   [&](const std::unique_ptr<ast::Binary>& bin) {
                       resolve(const_cast<ast::Expr&>(bin->left));
                       resolve(const_cast<ast::Expr&>(bin->right));
                   },
                   [&](const std::unique_ptr<ast::Variable>& var) {
                       if (scopes_.size() > 1) {
                           auto& scope = scopes_.back();
                           auto it = scope.find(var->name.lexeme);
                           if (it != scope.end() && !it->second) {
                               error(var->name, "Can't read local variable in its own initializer.");
                           }
                       }
                       resolve_local(expr, var->name);
                   },
                   [&](const std::unique_ptr<ast::Assign>& assign) {
                       resolve(const_cast<ast::Expr&>(assign->value));
                       resolve_local(expr, assign->name);
                   },
                   [&](const std::unique_ptr<ast::Logical>& logical) {
                       resolve(const_cast<ast::Expr&>(logical->left));
                       resolve(const_cast<ast::Expr&>(logical->right));
                   },
                   [&](const std::unique_ptr<ast::Call>& call) {
                       resolve(const_cast<ast::Expr&>(call->callee));
                       for (auto& arg : call->arguments) {
                           resolve(arg);
                       }
                   },
               },
               expr);
}

auto Resolver::begin_scope() -> void {
    scopes_.push_back({});
}

auto Resolver::end_scope() -> void {
    scopes_.pop_back();
}

auto Resolver::has_errors() const -> bool {
    return had_error_;
}

auto Resolver::error(const Token& token, std::string_view message) -> void {
    had_error_ = true;
    if (token.type == TokenType::EOF_) {
        std::cerr << "[line " << token.line << "] Error at end: " << message << '\n';
    } else {
        std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << '\n';
    }
}

auto Resolver::declare(const Token& name) -> void {
    if (scopes_.empty()) {
        return;
    }
    scopes_.back()[name.lexeme] = false;
}

auto Resolver::define(const Token& name) -> void {
    if (scopes_.empty()) {
        return;
    }
    scopes_.back()[name.lexeme] = true;
}

auto Resolver::resolve_local(const ast::Expr& expr, const Token& name) -> void {
    for (int i = static_cast<int>(scopes_.size()) - 1; i >= 0; --i) {
        if (scopes_[static_cast<std::size_t>(i)].count(name.lexeme) != 0) {
            int depth = static_cast<int>(scopes_.size()) - 1 - i;
            auto& mutable_expr = const_cast<ast::Expr&>(expr);
            if (auto* var = std::get_if<std::unique_ptr<ast::Variable>>(&mutable_expr)) {
                (*var)->depth = depth;
            } else if (auto* assign = std::get_if<std::unique_ptr<ast::Assign>>(&mutable_expr)) {
                (*assign)->depth = depth;
            }
            return;
        }
    }
}
