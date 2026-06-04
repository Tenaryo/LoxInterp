#include "interpreter/interpreter.hpp"

#include <ctime>
#include <iostream>

#include "scanner/scanner.hpp"
#include "util/format.hpp"
#include "util/overloaded.hpp"

namespace {

struct NativeClock : Callable {
    auto call(Environment& /*env*/, const std::vector<LoxLiteral>& /*args*/) -> LoxLiteral override {
        return static_cast<double>(std::time(nullptr));
    }
    auto to_string() const -> std::string override {
        return "<native fn>";
    }
};

} // namespace

auto Function::call(Environment& /*env*/, const std::vector<LoxLiteral>& args) -> LoxLiteral {
    Environment func_env(closure);
    for (std::size_t i = 0; i < params.size(); ++i) {
        func_env.define(params[i].lexeme, args[i]);
    }
    try {
        for (const auto& stmt : body) {
            execute(stmt, func_env);
        }
    } catch (const Return& ret) {
        return ret.value;
    }
    return std::monostate{};
}

auto Function::to_string() const -> std::string {
    return "<fn " + name.lexeme + ">";
}

Environment::Environment(Environment* enclosing) : enclosing_(enclosing) {
}

auto Environment::define(const std::string& name, LoxLiteral value) -> void {
    values_[name] = std::move(value);
}

auto Environment::assign(const Token& name, LoxLiteral value) -> void {
    auto it = values_.find(name.lexeme);
    if (it != values_.end()) {
        it->second = std::move(value);
        return;
    }
    if (enclosing_ != nullptr) {
        enclosing_->assign(name, std::move(value));
        return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

auto Environment::get(const Token& name) -> LoxLiteral {
    auto it = values_.find(name.lexeme);
    if (it != values_.end()) {
        return it->second;
    }
    if (enclosing_ != nullptr) {
        return enclosing_->get(name);
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
            [&](const std::unique_ptr<ast::Assign>& assign) -> LoxLiteral {
                auto value = evaluate(assign->value, env);
                env.assign(assign->name, std::move(value));
                return value;
            },
            [&](const std::unique_ptr<ast::Logical>& logical) -> LoxLiteral {
                auto left = evaluate(logical->left, env);
                if (logical->op.type == TokenType::OR) {
                    bool is_falsy = std::holds_alternative<std::monostate>(left)
                                    || (std::holds_alternative<bool>(left) && !std::get<bool>(left));
                    if (!is_falsy) {
                        return left;
                    }
                } else {
                    bool is_truthy = !(std::holds_alternative<std::monostate>(left)
                                       || (std::holds_alternative<bool>(left) && !std::get<bool>(left)));
                    if (!is_truthy) {
                        return left;
                    }
                }
                return evaluate(logical->right, env);
            },
            [&](const std::unique_ptr<ast::Call>& call) -> LoxLiteral {
                auto callee = evaluate(call->callee, env);
                std::vector<LoxLiteral> args;
                args.reserve(call->arguments.size());
                for (auto& arg : call->arguments) {
                    args.push_back(evaluate(arg, env));
                }
                auto* callable = std::get_if<std::shared_ptr<Callable>>(&callee);
                if (callable == nullptr || *callable == nullptr) {
                    throw RuntimeError(call->paren, "Can only call functions and classes.");
                }
                return (*callable)->call(env, args);
            },
        },
        expr);
}

auto interpret(const std::vector<ast::Stmt>& statements) -> void {
    Environment global;
    global.define("clock", std::make_shared<NativeClock>());
    for (const auto& stmt : statements) {
        execute(stmt, global);
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
                   [&](const std::unique_ptr<ast::BlockStmt>& block) {
                       Environment block_env(&env);
                       for (const auto& s : block->statements) {
                           execute(s, block_env);
                       }
                   },
                   [&](const std::unique_ptr<ast::IfStmt>& if_stmt) {
                       auto cond = evaluate(if_stmt->condition, env);
                       bool is_truthy = !(std::holds_alternative<std::monostate>(cond)
                                          || (std::holds_alternative<bool>(cond) && !std::get<bool>(cond)));
                       if (is_truthy) {
                           execute(if_stmt->then_branch, env);
                       } else if (if_stmt->else_branch.has_value()) {
                           execute(*if_stmt->else_branch, env);
                       }
                   },
                   [&](const std::unique_ptr<ast::WhileStmt>& while_stmt) {
                       while (true) {
                           auto cond = evaluate(while_stmt->condition, env);
                           bool is_truthy = !(std::holds_alternative<std::monostate>(cond)
                                              || (std::holds_alternative<bool>(cond) && !std::get<bool>(cond)));
                           if (!is_truthy) {
                               break;
                           }
                           execute(while_stmt->body, env);
                       }
                   },
                   [&](const std::unique_ptr<ast::FunctionStmt>& func) {
                       auto& f = const_cast<ast::FunctionStmt&>(*func);
                       auto fn = std::make_shared<Function>();
                       fn->name = f.name;
                       fn->params = std::move(f.params);
                       fn->body = std::move(f.body);
                       fn->closure = &env;
                       env.define(fn->name.lexeme, fn);
                   },
                   [&](const std::unique_ptr<ast::ReturnStmt>& ret) {
                       LoxLiteral value = std::monostate{};
                       if (ret->value.has_value()) {
                           value = evaluate(*ret->value, env);
                       }
                       throw Return(std::move(value));
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
        return format_double_numeric(*val);
    }
    if (const auto* str = std::get_if<std::string>(&value)) {
        return *str;
    }
    if (const auto* callable = std::get_if<std::shared_ptr<Callable>>(&value)) {
        if (*callable != nullptr) {
            return (*callable)->to_string();
        }
        return "nil";
    }
    return "nil";
}
