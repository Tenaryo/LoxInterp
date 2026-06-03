#pragma once

#include "parser/parser.hpp"

[[nodiscard]] auto evaluate(const ast::Expr& expr) -> LoxLiteral;

auto format_value(const LoxLiteral& value) -> std::string;
