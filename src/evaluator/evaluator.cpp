#include "evaluator/evaluator.hpp"

#include <cmath>

#include "util/overloaded.hpp"

auto evaluate(const ast::Expr& expr) -> LoxLiteral {
    return std::visit(
        overloaded{
            [](const ast::Literal& lit) -> LoxLiteral { return lit.value; },
            [](const std::unique_ptr<ast::Grouping>& grp) -> LoxLiteral { return evaluate(grp->expression); },
            [](const auto&) -> LoxLiteral { return std::monostate{}; },
        },
        expr);
}

auto format_value(const LoxLiteral& value) -> std::string {
    if (std::holds_alternative<std::monostate>(value)) {
        return "nil";
    }
    if (const auto* b = std::get_if<bool>(&value)) {
        return *b ? "true" : "false";
    }
    if (const auto* val = std::get_if<double>(&value)) {
        double v = *val;
        if (v == std::floor(v) && !std::isinf(v)) {
            return std::to_string(static_cast<long long>(v));
        }
        std::string s = std::to_string(v);
        s.erase(s.find_last_not_of('0') + 1);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
    }
    if (const auto* str = std::get_if<std::string>(&value)) {
        return *str;
    }
    return "nil";
}
