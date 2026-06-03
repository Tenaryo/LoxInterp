#include <gtest/gtest.h>

#include "parser.hpp"
#include "scanner.hpp"

TEST(ParserTest, ParseTrue) {
    Scanner scanner("true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "true");
}

TEST(ParserTest, ParseFalse) {
    Scanner scanner("false");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "false");
}

TEST(ParserTest, ParseNil) {
    Scanner scanner("nil");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "nil");
}

TEST(ParserTest, ParseInteger) {
    Scanner scanner("42");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "42.0");
}

TEST(ParserTest, ParseDecimal) {
    Scanner scanner("12.34");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "12.34");
}

TEST(ParserTest, ParseGrouping) {
    Scanner scanner("(\"foo\")");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(group foo)");
}

TEST(ParserTest, ParseNestedGrouping) {
    Scanner scanner("((true))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(group (group true))");
}

TEST(ParserTest, ParseUnaryBang) {
    Scanner scanner("!true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(! true)");
}

TEST(ParserTest, ParseUnaryMinus) {
    Scanner scanner("-42");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(- 42.0)");
}

TEST(ParserTest, ParseDoubleUnary) {
    Scanner scanner("!!false");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(! (! false))");
}

TEST(ParserTest, ParseMultiplication) {
    Scanner scanner("16 * 38");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(* 16.0 38.0)");
}

TEST(ParserTest, ParseDivision) {
    Scanner scanner("16 * 38 / 58");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(/ (* 16.0 38.0) 58.0)");
}

TEST(ParserTest, ParseAddition) {
    Scanner scanner("52 + 80 - 94");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(- (+ 52.0 80.0) 94.0)");
}

TEST(ParserTest, ParseAddMultPrecedence) {
    Scanner scanner("2 + 3 * 4");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(+ 2.0 (* 3.0 4.0))");
}

TEST(ParserTest, ParseComparison) {
    Scanner scanner("83 < 99 < 115");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(< (< 83.0 99.0) 115.0)");
}

TEST(ParserTest, ParseMixedComparison) {
    Scanner scanner("1 <= 2");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto expr = parser.parse();
    EXPECT_EQ(print_ast(expr), "(<= 1.0 2.0)");
}
