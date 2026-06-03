#pragma once

#include <cmath>
#include <string>

inline auto format_double_numeric(double v) -> std::string {
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

inline auto format_double_ast(double v) -> std::string {
    if (v == std::floor(v) && !std::isinf(v)) {
        return std::to_string(static_cast<long long>(v)) + ".0";
    }
    std::string s = std::to_string(v);
    s.erase(s.find_last_not_of('0') + 1);
    if (s.back() == '.') {
        s += '0';
    }
    return s;
}
