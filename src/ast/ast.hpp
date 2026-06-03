#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "scanner/scanner.hpp"

namespace ast {

struct Binary;
struct Grouping;
struct Unary;
struct Variable;

struct Literal {
    LoxLiteral value;
};

using Expr = std::variant<Literal,
                          std::unique_ptr<Binary>,
                          std::unique_ptr<Grouping>,
                          std::unique_ptr<Unary>,
                          std::unique_ptr<Variable>>;

struct Binary {
    Expr left;
    Token op;
    Expr right;
};

struct Grouping {
    Expr expression;
};

struct Unary {
    Token op;
    Expr right;
};

struct PrintStmt;
struct ExprStmt;
struct VarStmt;

using Stmt = std::variant<std::unique_ptr<PrintStmt>, std::unique_ptr<ExprStmt>, std::unique_ptr<VarStmt>>;

struct PrintStmt {
    Expr expression;
};

struct ExprStmt {
    Expr expression;
};

struct VarStmt {
    Token name;
    Expr initializer;
};

struct Variable {
    Token name;
};

} // namespace ast
