#include "scanner.hpp"

#include <iostream>

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

auto Scanner::has_errors() const -> bool {
    return had_error_;
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
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line_++;
        break;
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
    case ',':
        add_token(TokenType::COMMA);
        break;
    case '.':
        add_token(TokenType::DOT);
        break;
    case '-':
        add_token(TokenType::MINUS);
        break;
    case '+':
        add_token(TokenType::PLUS);
        break;
    case ';':
        add_token(TokenType::SEMICOLON);
        break;
    case '*':
        add_token(TokenType::STAR);
        break;
    case '!':
        add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '/':
        if (match('/')) {
            while (!is_at_end() && advance() != '\n') {
            }
        } else {
            add_token(TokenType::SLASH);
        }
        break;
    default:
        error(line_, std::string("Unexpected character: ") + ch);
        break;
    }
}

auto Scanner::error(int line, std::string_view message) -> void {
    had_error_ = true;
    std::cerr << "[line " << line << "] Error: " << message << '\n';
}

auto Scanner::match(char expected) -> bool {
    if (is_at_end()) {
        return false;
    }
    if (source_[current_] != expected) {
        return false;
    }
    current_++;
    return true;
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
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::DOT:
            return "DOT";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::STAR:
            return "STAR";
        case TokenType::BANG:
            return "BANG";
        case TokenType::BANG_EQUAL:
            return "BANG_EQUAL";
        case TokenType::EQUAL:
            return "EQUAL";
        case TokenType::EQUAL_EQUAL:
            return "EQUAL_EQUAL";
        case TokenType::GREATER:
            return "GREATER";
        case TokenType::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TokenType::LESS:
            return "LESS";
        case TokenType::LESS_EQUAL:
            return "LESS_EQUAL";
        case TokenType::SLASH:
            return "SLASH";
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
