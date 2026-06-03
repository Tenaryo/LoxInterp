#include <gtest/gtest.h>

#include "evaluator/evaluator.hpp"
#include "parser/parser.hpp"
#include "scanner/scanner.hpp"

TEST(EvaluatorTest, EvaluateTrue) {
    Scanner scanner("true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateFalse) {
    Scanner scanner("false");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateNil) {
    Scanner scanner("nil");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "nil");
}

TEST(EvaluatorTest, EvaluateNumber) {
    Scanner scanner("10.40");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "10.4");
}

TEST(EvaluatorTest, EvaluateInteger) {
    Scanner scanner("10");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "10");
}

TEST(EvaluatorTest, EvaluateString) {
    Scanner scanner("\"hello world!\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateGrouping) {
    Scanner scanner("(\"hello world!\")");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateNestedGrouping) {
    Scanner scanner("((false))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}
