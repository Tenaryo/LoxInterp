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
