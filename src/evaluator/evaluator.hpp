#pragma once

#include <stdexcept>

#include "parser/parser.hpp"

struct RuntimeError : std::runtime_error {
    Token token;
    RuntimeError(const Token& tok, const std::string& msg);
};

auto evaluate(const ast::Expr& expr) -> LoxLiteral;

auto format_value(const LoxLiteral& value) -> std::string;
