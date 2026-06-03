#include "scanner.hpp"

#include <charconv>
#include <cmath>
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

auto Scanner::peek() const -> char {
    if (is_at_end()) {
        return '\0';
    }
    return source_[current_];
}

auto Scanner::peek_next() const -> char {
    if (current_ + 1 >= source_.size()) {
        return '\0';
    }
    return source_[current_ + 1];
}

auto Scanner::add_token(TokenType type) -> void {
    add_token(type, std::monostate{});
}

auto Scanner::add_token(TokenType type, LoxLiteral literal) -> void {
    std::string lexeme = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, std::move(lexeme), std::move(literal), line_});
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
    case '"': {
        while (peek() != '"' && !is_at_end()) {
            if (peek() == '\n') {
                line_++;
            }
            advance();
        }
        if (is_at_end()) {
            error(line_, "Unterminated string.");
            return;
        }
        advance();
        std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
        add_token(TokenType::STRING, std::move(value));
        break;
    }
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
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        while (is_digit(peek())) {
            advance();
        }
        if (peek() == '.' && is_digit(peek_next())) {
            advance();
            while (is_digit(peek())) {
                advance();
            }
        }
        double value = 0.0;
        std::from_chars(source_.data() + start_, source_.data() + current_, value);
        add_token(TokenType::NUMBER, value);
        break;
    }
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
            while (!is_at_end() && peek() != '\n') {
                advance();
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

auto Scanner::is_digit(char ch) const -> bool {
    return ch >= '0' && ch <= '9';
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
        case TokenType::STRING:
            return "STRING";
        case TokenType::NUMBER:
            return "NUMBER";
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
            double v = *val;
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
