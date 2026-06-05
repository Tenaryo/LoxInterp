#include "parser/parser.hpp"

#include <iostream>

#include "util/format.hpp"
#include "util/overloaded.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
}

auto Parser::parse() -> ast::Expr {
    try {
        return expression();
    } catch (const ParseError&) {
        return ast::Literal{std::monostate{}};
    }
}

auto Parser::parse_statements() -> std::vector<ast::Stmt> {
    std::vector<ast::Stmt> statements;
    while (!is_at_end()) {
        try {
            statements.push_back(declaration());
        } catch (const ParseError&) {
            synchronize();
        }
    }
    return statements;
}

auto Parser::declaration() -> ast::Stmt {
    if (match(TokenType::VAR)) {
        return var_declaration();
    }
    if (match(TokenType::FUN)) {
        return function_declaration();
    }
    if (match(TokenType::CLASS)) {
        return class_declaration();
    }
    return statement();
}

auto Parser::statement() -> ast::Stmt {
    if (match(TokenType::PRINT)) {
        return print_statement();
    }
    if (match(TokenType::LEFT_BRACE)) {
        return block();
    }
    if (match(TokenType::IF)) {
        return if_statement();
    }
    if (match(TokenType::WHILE)) {
        return while_statement();
    }
    if (match(TokenType::FOR)) {
        return for_statement();
    }
    if (match(TokenType::RETURN)) {
        return return_statement();
    }
    return expr_statement();
}

auto Parser::print_statement() -> ast::Stmt {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<ast::PrintStmt>(std::move(expr));
}

auto Parser::expr_statement() -> ast::Stmt {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ast::ExprStmt>(std::move(expr));
}

auto Parser::var_declaration() -> ast::Stmt {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    ast::Expr initializer = ast::Literal{std::monostate{}};
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<ast::VarStmt>(name, std::move(initializer));
}

auto Parser::function_declaration() -> ast::Stmt {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");

    auto body = block();
    return std::make_unique<ast::FunctionStmt>(
        name, std::move(params), std::move(std::get<std::unique_ptr<ast::BlockStmt>>(body)->statements));
}

auto Parser::class_declaration() -> ast::Stmt {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");

    std::optional<ast::Expr> superclass;
    if (match(TokenType::LESS)) {
        Token super_name = consume(TokenType::IDENTIFIER, "Expect superclass name.");
        superclass = std::make_unique<ast::Variable>(super_name);
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::unique_ptr<ast::FunctionStmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        match(TokenType::FUN);
        auto func = function_declaration();
        auto* func_ptr = std::get_if<std::unique_ptr<ast::FunctionStmt>>(&func);
        methods.push_back(std::move(*func_ptr));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    return std::make_unique<ast::ClassStmt>(name, std::move(superclass), std::move(methods));
}

auto Parser::block() -> ast::Stmt {
    std::vector<ast::Stmt> statements;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<ast::BlockStmt>(std::move(statements));
}

auto Parser::if_statement() -> ast::Stmt {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto then_branch = statement();
    std::optional<ast::Stmt> else_branch;
    if (match(TokenType::ELSE)) {
        else_branch = statement();
    }
    return std::make_unique<ast::IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

auto Parser::while_statement() -> ast::Stmt {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    auto body = statement();
    return std::make_unique<ast::WhileStmt>(std::move(condition), std::move(body));
}

auto Parser::for_statement() -> ast::Stmt {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::optional<ast::Stmt> initializer;
    if (match(TokenType::SEMICOLON)) {
    } else if (match(TokenType::VAR)) {
        initializer = var_declaration();
    } else {
        initializer = expr_statement();
    }

    std::optional<ast::Expr> condition;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::optional<ast::Expr> increment;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    std::vector<ast::Stmt> while_body_stmts;
    while_body_stmts.push_back(std::move(body));
    if (increment.has_value()) {
        while_body_stmts.push_back(std::make_unique<ast::ExprStmt>(std::move(*increment)));
    }

    ast::Expr cond = condition.has_value() ? std::move(*condition) : ast::Literal{true};
    auto while_stmt = std::make_unique<ast::WhileStmt>(std::move(cond),
                                                       std::make_unique<ast::BlockStmt>(std::move(while_body_stmts)));

    if (initializer.has_value()) {
        std::vector<ast::Stmt> outer_stmts;
        outer_stmts.push_back(std::move(*initializer));
        outer_stmts.push_back(std::move(while_stmt));
        return std::make_unique<ast::BlockStmt>(std::move(outer_stmts));
    }
    return while_stmt;
}

auto Parser::return_statement() -> ast::Stmt {
    Token keyword = previous();
    std::optional<ast::Expr> value;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ast::ReturnStmt>(keyword, std::move(value));
}

auto Parser::has_errors() const -> bool {
    return had_error_;
}

auto Parser::expression() -> ast::Expr {
    return assignment();
}

auto Parser::assignment() -> ast::Expr {
    auto expr = logic_or();
    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();
        if (auto* var = std::get_if<std::unique_ptr<ast::Variable>>(&expr)) {
            return std::make_unique<ast::Assign>((*var)->name, std::move(value));
        }
        if (auto* get = std::get_if<std::unique_ptr<ast::Get>>(&expr)) {
            return std::make_unique<ast::Set>(
                std::move(const_cast<ast::Expr&>((*get)->object)), (*get)->name, std::move(value));
        }
        error(equals, "Invalid assignment target.");
    }
    return expr;
}

auto Parser::logic_or() -> ast::Expr {
    auto expr = logic_and();
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = logic_and();
        expr = std::make_unique<ast::Logical>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::logic_and() -> ast::Expr {
    auto expr = equality();
    while (match(TokenType::AND)) {
        Token op = previous();
        auto right = equality();
        expr = std::make_unique<ast::Logical>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::equality() -> ast::Expr {
    auto expr = comparison();
    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::comparison() -> ast::Expr {
    auto expr = term();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS)
           || match(TokenType::LESS_EQUAL)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::term() -> ast::Expr {
    auto expr = factor();
    while (match(TokenType::MINUS) || match(TokenType::PLUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::factor() -> ast::Expr {
    auto expr = unary();
    while (match(TokenType::SLASH) || match(TokenType::STAR)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<ast::Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

auto Parser::unary() -> ast::Expr {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<ast::Unary>(op, std::move(right));
    }
    return call();
}

auto Parser::call() -> ast::Expr {
    auto expr = primary();
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finish_call(std::move(expr));
        } else if (match(TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<ast::Get>(std::move(expr), name);
        } else {
            break;
        }
    }
    return expr;
}

auto Parser::finish_call(ast::Expr callee) -> ast::Expr {
    std::vector<ast::Expr> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<ast::Call>(std::move(callee), paren, std::move(arguments));
}

auto Parser::primary() -> ast::Expr {
    if (match(TokenType::FALSE_)) {
        return ast::Literal{false};
    }
    if (match(TokenType::TRUE_)) {
        return ast::Literal{true};
    }
    if (match(TokenType::NIL)) {
        return ast::Literal{std::monostate{}};
    }
    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
        return ast::Literal{to_lox_literal(previous().literal)};
    }
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<ast::Variable>(previous());
    }
    if (match(TokenType::THIS)) {
        return std::make_unique<ast::ThisExpr>(previous());
    }
    if (match(TokenType::SUPER)) {
        Token keyword = previous();
        consume(TokenType::DOT, "Expect '.' after 'super'.");
        Token method = consume(TokenType::IDENTIFIER, "Expect superclass method name.");
        return std::make_unique<ast::SuperExpr>(keyword, method);
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<ast::Grouping>(std::move(expr));
    }
    throw error(peek(), "Expect expression.");
}

auto Parser::match(TokenType type) -> bool {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

auto Parser::check(TokenType type) -> bool {
    if (is_at_end()) {
        return false;
    }
    return peek().type == type;
}

auto Parser::advance() -> Token {
    if (!is_at_end()) {
        current_++;
    }
    return previous();
}

auto Parser::is_at_end() const -> bool {
    return peek().type == TokenType::EOF_;
}

auto Parser::peek() const -> Token {
    return tokens_[current_];
}

auto Parser::previous() -> Token {
    return tokens_[current_ - 1];
}

auto Parser::error(const Token& token, std::string_view message) -> ParseError {
    had_error_ = true;
    if (token.type == TokenType::EOF_) {
        std::cerr << "[line " << token.line << "] Error at end: " << message << '\n';
    } else {
        std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << '\n';
    }
    return ParseError{};
}

auto Parser::consume(TokenType type, std::string_view message) -> Token {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

auto Parser::synchronize() -> void {
    if (!is_at_end()) {
        current_++;
    }

    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) {
            return;
        }
        switch (peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
        default:
            break;
        }
        current_++;
    }
}

auto print_ast(const ast::Expr& expr) -> std::string {
    auto format_literal = [](const LoxLiteral& literal) -> std::string {
        if (std::holds_alternative<std::monostate>(literal)) {
            return "nil";
        }
        if (const auto* b = std::get_if<bool>(&literal)) {
            return *b ? "true" : "false";
        }
        if (const auto* val = std::get_if<double>(&literal)) {
            return format_double_ast(*val);
        }
        if (const auto* str = std::get_if<std::string>(&literal)) {
            return *str;
        }
        return "nil";
    };

    return std::visit(overloaded{
                          [&](const ast::Literal& lit) -> std::string { return format_literal(lit.value); },
                          [&](const std::unique_ptr<ast::Grouping>& grp) -> std::string {
                              return "(group " + print_ast(grp->expression) + ")";
                          },
                          [&](const std::unique_ptr<ast::Unary>& un) -> std::string {
                              return "(" + un->op.lexeme + " " + print_ast(un->right) + ")";
                          },
                          [&](const std::unique_ptr<ast::Binary>& bin) -> std::string {
                              return "(" + bin->op.lexeme + " " + print_ast(bin->left) + " " + print_ast(bin->right)
                                     + ")";
                          },
                          [&](const std::unique_ptr<ast::Variable>& var) -> std::string { return var->name.lexeme; },
                          [&](const std::unique_ptr<ast::Assign>& assign) -> std::string {
                              return assign->name.lexeme + " = " + print_ast(assign->value);
                          },
                          [&](const std::unique_ptr<ast::Logical>& logical) -> std::string {
                              return "(" + print_ast(logical->left) + " " + logical->op.lexeme + " "
                                     + print_ast(logical->right) + ")";
                          },
                          [&](const std::unique_ptr<ast::Call>& call) -> std::string {
                              std::string result = print_ast(call->callee) + "(";
                              for (std::size_t i = 0; i < call->arguments.size(); ++i) {
                                  if (i > 0) {
                                      result += ", ";
                                  }
                                  result += print_ast(call->arguments[i]);
                              }
                              return result + ")";
                          },
                          [&](const std::unique_ptr<ast::Get>& get) -> std::string {
                              return print_ast(get->object) + "." + get->name.lexeme;
                          },
                          [&](const std::unique_ptr<ast::Set>& set) -> std::string {
                              return print_ast(set->object) + "." + set->name.lexeme + " = " + print_ast(set->value);
                          },
                          [&](const std::unique_ptr<ast::ThisExpr>& /*this_expr*/) -> std::string { return "this"; },
                          [&](const std::unique_ptr<ast::SuperExpr>& super) -> std::string {
                              return "super." + super->method.lexeme;
                          },
                      },
                      expr);
}
