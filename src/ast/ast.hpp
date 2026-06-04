#pragma once

#include <memory>
#include <optional>
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
struct Logical;
struct Call;

struct Literal {
    LoxLiteral value;
};

using Expr = std::variant<Literal,
                          std::unique_ptr<Binary>,
                          std::unique_ptr<Grouping>,
                          std::unique_ptr<Unary>,
                          std::unique_ptr<Variable>,
                          std::unique_ptr<Assign>,
                          std::unique_ptr<Logical>,
                          std::unique_ptr<Call>>;

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

struct Logical {
    Expr left;
    Token op;
    Expr right;
};

struct Call {
    Expr callee;
    Token paren;
    std::vector<Expr> arguments;
};

// Statements

struct PrintStmt;
struct ExprStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;

using Stmt = std::variant<std::unique_ptr<PrintStmt>,
                          std::unique_ptr<ExprStmt>,
                          std::unique_ptr<VarStmt>,
                          std::unique_ptr<BlockStmt>,
                          std::unique_ptr<IfStmt>,
                          std::unique_ptr<WhileStmt>>;

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

struct IfStmt {
    Expr condition;
    Stmt then_branch;
    std::optional<Stmt> else_branch;
};

struct WhileStmt {
    Expr condition;
    Stmt body;
};

} // namespace ast
