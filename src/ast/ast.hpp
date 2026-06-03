#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "scanner/scanner.hpp"

namespace ast {

// Expressions

struct Binary;
struct Grouping;
struct Unary;
struct Variable;
struct Assign;

struct Literal {
    LoxLiteral value;
};

using Expr = std::variant<Literal,
                          std::unique_ptr<Binary>,
                          std::unique_ptr<Grouping>,
                          std::unique_ptr<Unary>,
                          std::unique_ptr<Variable>,
                          std::unique_ptr<Assign>>;

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

struct Variable {
    Token name;
};

struct Assign {
    Token name;
    Expr value;
};

// Statements

struct PrintStmt;
struct ExprStmt;
struct VarStmt;
struct BlockStmt;

using Stmt = std::variant<std::unique_ptr<PrintStmt>,
                          std::unique_ptr<ExprStmt>,
                          std::unique_ptr<VarStmt>,
                          std::unique_ptr<BlockStmt>>;

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

struct BlockStmt {
    std::vector<Stmt> statements;
};

} // namespace ast
