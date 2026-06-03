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

TEST(EvaluatorTest, EvaluateUnaryNegation) {
    Scanner scanner("-73");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "-73");
}

TEST(EvaluatorTest, EvaluateUnaryBangTrue) {
    Scanner scanner("!true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateUnaryBangNumber) {
    Scanner scanner("!10.40");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateUnaryBangNestedFalse) {
    Scanner scanner("!((false))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateMultiply) {
    Scanner scanner("18 * 3 / (3 * 6)");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "3");
}

TEST(EvaluatorTest, EvaluateDivide) {
    Scanner scanner("42 / 5");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "8.4");
}

TEST(EvaluatorTest, EvaluateAddSubtract) {
    Scanner scanner("20 + 74 - (-(14 - 33))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "75");
}

TEST(EvaluatorTest, EvaluateStringConcat) {
    Scanner scanner("\"hello\" + \" world!\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateGreater) {
    Scanner scanner("57 > -65");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateGreaterEqual) {
    Scanner scanner("11 >= 11");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateEqual) {
    Scanner scanner("\"foo\" == \"foo\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateNotEqual) {
    Scanner scanner("\"foo\" != \"bar\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateEqualDifferentTypes) {
    Scanner scanner("61 == \"61\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, RuntimeErrorUnaryMinusOnString) {
    Scanner scanner("-\"foo\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    EXPECT_THROW(evaluate(parser.parse()), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorUnaryMinusOnBool) {
    Scanner scanner("-true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    EXPECT_THROW(evaluate(parser.parse()), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorBinaryMultiplyOnString) {
    Scanner scanner("\"foo\" * 42");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    EXPECT_THROW(evaluate(parser.parse()), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorBinaryDivideOnBool) {
    Scanner scanner("true / 2");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    EXPECT_THROW(evaluate(parser.parse()), RuntimeError);
}

TEST(EvaluatorTest, ComprehensiveEvalArith) {
    Scanner scanner("-(3 * 4) > 2 == true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, ComprehensiveEvalStringConcatEq) {
    Scanner scanner("(\"foo\" + \"bar\") == \"foobar\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto result = evaluate(parser.parse());
    EXPECT_EQ(format_value(result), "true");
}
