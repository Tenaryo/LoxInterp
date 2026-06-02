#include "scanner.hpp"

Scanner::Scanner(std::string source) : source_(std::move(source)) {
}

auto Scanner::scan_tokens() -> std::vector<Token> {
    while (!is_at_end()) {
        start_ = current_;
        scan_token();
    }
    tokens_.push_back({TokenType::EOF_, "", std::monostate{}, line_});
    return std::move(tokens_);
}

auto Scanner::is_at_end() const -> bool {
    return current_ >= source_.size();
}

auto Scanner::advance() -> char {
    return source_[current_++];
}

auto Scanner::add_token(TokenType type) -> void {
    std::string lexeme = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, std::move(lexeme), std::monostate{}, line_});
}

auto Scanner::scan_token() -> void {
    char ch = advance();
    switch (ch) {
    case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
    case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
    case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
    case '}':
        add_token(TokenType::RIGHT_BRACE);
        break;
    default:
        break;
    }
}

auto format_token(const Token& token) -> std::string {
    auto type_name = [](TokenType type) -> std::string_view {
        switch (type) {
        case TokenType::LEFT_PAREN:
            return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:
            return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:
            return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:
            return "RIGHT_BRACE";
        case TokenType::EOF_:
            return "EOF";
        }
        return "UNKNOWN";
    };

    auto literal_str = [](const LoxLiteral& lit) -> std::string {
        if (std::holds_alternative<std::monostate>(lit)) {
            return "null";
        }
        if (const auto* str = std::get_if<std::string>(&lit)) {
            return *str;
        }
        if (const auto* val = std::get_if<double>(&lit)) {
            return std::to_string(*val);
        }
        return "null";
    };

    std::string result;
    result.reserve(64);
    result += type_name(token.type);
    result += ' ';
    result += token.lexeme;
    result += ' ';
    result += literal_str(token.literal);
    return result;
}
