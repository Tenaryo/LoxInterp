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
