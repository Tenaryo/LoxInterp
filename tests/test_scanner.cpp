#include <gtest/gtest.h>

#include "scanner.hpp"

TEST(ScannerTest, EmptyInput) {
    Scanner scanner("");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::EOF_);
    EXPECT_TRUE(tokens[0].lexeme.empty());
}

TEST(ScannerTest, SingleLeftParen) {
    Scanner scanner("(");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[0].lexeme, "(");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleRightParen) {
    Scanner scanner(")");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[0].lexeme, ")");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleLeftBrace) {
    Scanner scanner("{");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[0].lexeme, "{");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleRightBrace) {
    Scanner scanner("}");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[0].lexeme, "}");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleComma) {
    Scanner scanner(",");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::COMMA);
    EXPECT_EQ(tokens[0].lexeme, ",");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleDot) {
    Scanner scanner(".");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::DOT);
    EXPECT_EQ(tokens[0].lexeme, ".");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleMinus) {
    Scanner scanner("-");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::MINUS);
    EXPECT_EQ(tokens[0].lexeme, "-");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SinglePlus) {
    Scanner scanner("+");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::PLUS);
    EXPECT_EQ(tokens[0].lexeme, "+");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleSemicolon) {
    Scanner scanner(";");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[0].lexeme, ";");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleStar) {
    Scanner scanner("*");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STAR);
    EXPECT_EQ(tokens[0].lexeme, "*");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, NoErrorForValidInput) {
    Scanner scanner("(,.-+)");
    scanner.scan_tokens();
    EXPECT_FALSE(scanner.has_errors());
}

TEST(ScannerTest, ErrorForUnknownCharacter) {
    Scanner scanner("$");
    scanner.scan_tokens();
    EXPECT_TRUE(scanner.has_errors());
}

TEST(ScannerTest, ValidTokensProducedDespiteErrors) {
    Scanner scanner("(,$#)");
    auto tokens = scanner.scan_tokens();
    EXPECT_TRUE(scanner.has_errors());
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::COMMA);
    EXPECT_EQ(tokens[2].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[3].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleBang) {
    Scanner scanner("!");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::BANG);
    EXPECT_EQ(tokens[0].lexeme, "!");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, BangEqual) {
    Scanner scanner("!=");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[0].lexeme, "!=");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleEqual) {
    Scanner scanner("=");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[0].lexeme, "=");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, EqualEqual) {
    Scanner scanner("==");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[0].lexeme, "==");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleLess) {
    Scanner scanner("<");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::LESS);
    EXPECT_EQ(tokens[0].lexeme, "<");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, LessEqual) {
    Scanner scanner("<=");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[0].lexeme, "<=");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleGreater) {
    Scanner scanner(">");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::GREATER);
    EXPECT_EQ(tokens[0].lexeme, ">");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, GreaterEqual) {
    Scanner scanner(">=");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[0].lexeme, ">=");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, SingleSlash) {
    Scanner scanner("/");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::SLASH);
    EXPECT_EQ(tokens[0].lexeme, "/");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
}

TEST(ScannerTest, CommentIgnored) {
    Scanner scanner("// comment");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::EOF_);
}

TEST(ScannerTest, SlashCommentNotConfused) {
    Scanner scanner("()// comment");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}

TEST(ScannerTest, SlashFollowedByCode) {
    Scanner scanner("/()");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::SLASH);
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[3].type, TokenType::EOF_);
}

TEST(ScannerTest, WhitespaceOnly) {
    Scanner scanner(" \t\r\n");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::EOF_);
}

TEST(ScannerTest, TokensWithSpaces) {
    Scanner scanner(" ( ) ");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}

TEST(ScannerTest, TokensWithNewlines) {
    Scanner scanner("(\n)");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}

TEST(ScannerTest, MultiLineErrors) {
    Scanner scanner("# (\n)\t@");
    auto tokens = scanner.scan_tokens();
    EXPECT_TRUE(scanner.has_errors());
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}

TEST(ScannerTest, CommentDoesNotAffectLineCount) {
    Scanner scanner("// comment\n#");
    scanner.scan_tokens();
    EXPECT_TRUE(scanner.has_errors());
}

TEST(ScannerTest, CommentAfterCodeDoesNotSkipNewline) {
    Scanner scanner("(\n// comment\n)");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}

TEST(ScannerTest, StringLiteral) {
    Scanner scanner("\"foo baz\"");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"foo baz\"");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
    auto* val = std::get_if<std::string>(&tokens[0].literal);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, "foo baz");
}

TEST(ScannerTest, EmptyString) {
    Scanner scanner("\"\"");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"\"");
    auto* val = std::get_if<std::string>(&tokens[0].literal);
    ASSERT_NE(val, nullptr);
    EXPECT_TRUE(val->empty());
}

TEST(ScannerTest, UnterminatedString) {
    Scanner scanner("\"bar");
    auto tokens = scanner.scan_tokens();
    EXPECT_TRUE(scanner.has_errors());
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::EOF_);
}

TEST(ScannerTest, IntegerLiteral) {
    Scanner scanner("42");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "42");
    EXPECT_EQ(tokens[1].type, TokenType::EOF_);
    auto* val = std::get_if<double>(&tokens[0].literal);
    ASSERT_NE(val, nullptr);
    EXPECT_DOUBLE_EQ(*val, 42.0);
}

TEST(ScannerTest, DecimalLiteral) {
    Scanner scanner("1234.1234");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "1234.1234");
    auto* val = std::get_if<double>(&tokens[0].literal);
    ASSERT_NE(val, nullptr);
    EXPECT_DOUBLE_EQ(*val, 1234.1234);
}

TEST(ScannerTest, NumberTrailingDot) {
    Scanner scanner("42.");
    auto tokens = scanner.scan_tokens();
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "42");
    EXPECT_EQ(tokens[1].type, TokenType::DOT);
    EXPECT_EQ(tokens[2].type, TokenType::EOF_);
}
