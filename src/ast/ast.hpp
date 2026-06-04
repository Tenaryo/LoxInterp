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
struct Get;
struct Set;

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
                          std::unique_ptr<Call>,
                          std::unique_ptr<Get>,
                          std::unique_ptr<Set>>;

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
    int depth = -1;
};

struct Assign {
    Token name;
    Expr value;
    int depth = -1;
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

struct Get {
    Expr object;
    Token name;
};

struct Set {
    Expr object;
    Token name;
    Expr value;
};

// Statements

struct PrintStmt;
struct ExprStmt;
struct VarStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;

using Stmt = std::variant<std::unique_ptr<PrintStmt>,
                          std::unique_ptr<ExprStmt>,
                          std::unique_ptr<VarStmt>,
                          std::unique_ptr<BlockStmt>,
                          std::unique_ptr<IfStmt>,
                          std::unique_ptr<WhileStmt>,
                          std::unique_ptr<FunctionStmt>,
                          std::unique_ptr<ReturnStmt>,
                          std::unique_ptr<ClassStmt>>;

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

struct FunctionStmt {
    Token name;
    std::vector<Token> params;
    std::vector<Stmt> body;
};

struct ReturnStmt {
    Token keyword;
    std::optional<Expr> value;
};

struct ClassStmt {
    Token name;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
};

} // namespace ast
