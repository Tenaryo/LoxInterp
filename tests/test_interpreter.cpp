#include <gtest/gtest.h>

#include "interpreter/interpreter.hpp"
#include "parser/parser.hpp"
#include "resolver/resolver.hpp"
#include "scanner/scanner.hpp"

TEST(EvaluatorTest, EvaluateTrue) {
    Scanner scanner("true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateFalse) {
    Scanner scanner("false");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateNil) {
    Scanner scanner("nil");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "nil");
}

TEST(EvaluatorTest, EvaluateNumber) {
    Scanner scanner("10.40");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "10.4");
}

TEST(EvaluatorTest, EvaluateInteger) {
    Scanner scanner("10");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "10");
}

TEST(EvaluatorTest, EvaluateString) {
    Scanner scanner("\"hello world!\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateGrouping) {
    Scanner scanner("(\"hello world!\")");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateNestedGrouping) {
    Scanner scanner("((false))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateUnaryNegation) {
    Scanner scanner("-73");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "-73");
}

TEST(EvaluatorTest, EvaluateUnaryBangTrue) {
    Scanner scanner("!true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateUnaryBangNumber) {
    Scanner scanner("!10.40");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, EvaluateUnaryBangNestedFalse) {
    Scanner scanner("!((false))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateMultiply) {
    Scanner scanner("18 * 3 / (3 * 6)");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "3");
}

TEST(EvaluatorTest, EvaluateDivide) {
    Scanner scanner("42 / 5");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "8.4");
}

TEST(EvaluatorTest, EvaluateAddSubtract) {
    Scanner scanner("20 + 74 - (-(14 - 33))");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "75");
}

TEST(EvaluatorTest, EvaluateStringConcat) {
    Scanner scanner("\"hello\" + \" world!\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "hello world!");
}

TEST(EvaluatorTest, EvaluateGreater) {
    Scanner scanner("57 > -65");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateGreaterEqual) {
    Scanner scanner("11 >= 11");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateEqual) {
    Scanner scanner("\"foo\" == \"foo\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateNotEqual) {
    Scanner scanner("\"foo\" != \"bar\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, EvaluateEqualDifferentTypes) {
    Scanner scanner("61 == \"61\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, RuntimeErrorUnaryMinusOnString) {
    Scanner scanner("-\"foo\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    EXPECT_THROW(evaluate(parser.parse(), env), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorUnaryMinusOnBool) {
    Scanner scanner("-true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    EXPECT_THROW(evaluate(parser.parse(), env), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorBinaryMultiplyOnString) {
    Scanner scanner("\"foo\" * 42");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    EXPECT_THROW(evaluate(parser.parse(), env), RuntimeError);
}

TEST(EvaluatorTest, RuntimeErrorBinaryDivideOnBool) {
    Scanner scanner("true / 2");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    EXPECT_THROW(evaluate(parser.parse(), env), RuntimeError);
}

TEST(EvaluatorTest, ComprehensiveEvalArith) {
    Scanner scanner("-(3 * 4) > 2 == true");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "false");
}

TEST(EvaluatorTest, ComprehensiveEvalStringConcatEq) {
    Scanner scanner("(\"foo\" + \"bar\") == \"foobar\"");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto env = std::make_shared<Environment>();
    auto result = evaluate(parser.parse(), env);
    EXPECT_EQ(format_value(result), "true");
}

TEST(EvaluatorTest, RunPrintString) {
    Scanner scanner("print \"Hello, World!\";");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    ASSERT_EQ(statements.size(), 1);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Hello, World!\n");
}

TEST(EvaluatorTest, RunPrintNumber) {
    Scanner scanner("print 42;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    ASSERT_EQ(statements.size(), 1);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "42\n");
}

TEST(EvaluatorTest, RunPrintExpression) {
    Scanner scanner("print 12 + 24;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    ASSERT_EQ(statements.size(), 1);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "36\n");
}

TEST(EvaluatorTest, RunIfTrue) {
    Scanner scanner("if (true) print \"yes\";");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "yes\n");
}

TEST(EvaluatorTest, RunIfFalse) {
    Scanner scanner("if (false) print \"no\";");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(EvaluatorTest, RunIfElse) {
    Scanner scanner("if (false) print \"no\"; else print \"yes\";");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "yes\n");
}

TEST(EvaluatorTest, RunLogicalOr) {
    Scanner scanner("print false or \"ok\";");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "ok\n");
}

TEST(EvaluatorTest, RunLogicalAnd) {
    Scanner scanner("print true and 42;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "42\n");
}

TEST(EvaluatorTest, RunWhileLoop) {
    Scanner scanner("var x = 0; while (x < 3) { print x; x = x + 1; }");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "0\n1\n2\n");
}

TEST(EvaluatorTest, RunForLoop) {
    Scanner scanner("for (var i = 0; i < 2; i = i + 1) print i;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "0\n1\n");
}

TEST(EvaluatorTest, RunClock) {
    Scanner scanner("print clock() >= 0;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "true\n");
}

TEST(EvaluatorTest, RunFunction) {
    Scanner scanner("fun foo() { print 7; } foo();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "7\n");
}

TEST(EvaluatorTest, RunReturn) {
    Scanner scanner("fun foo() { return 42; } print foo();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "42\n");
}

TEST(EvaluatorTest, RunReturnNil) {
    Scanner scanner("fun foo() { return; } print foo();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "nil\n");
}

TEST(EvaluatorTest, RunClosure) {
    Scanner scanner("fun makeCounter() { var i = 0; fun count() { i = i + 1; print i; } return count; }"
                    "var c = makeCounter(); c(); c();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n");
}

TEST(EvaluatorTest, RunArityError) {
    Scanner scanner("fun f(a, b) {} f(1);");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    EXPECT_THROW(interpret(statements), RuntimeError);
}

TEST(EvaluatorTest, RunCallNonFunction) {
    Scanner scanner("42();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    EXPECT_THROW(interpret(statements), RuntimeError);
}

TEST(EvaluatorTest, RunResolvedClosure) {
    Scanner scanner("var x = \"global\";\n{\n  fun f() { print x; }\n  f();\n}");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "global\n");
}

TEST(EvaluatorTest, RunGlobalSelfInit) {
    Scanner scanner("var a = \"value\";\nvar a = a;\nprint a;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    EXPECT_FALSE(resolver.has_errors());
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "value\n");
}

TEST(EvaluatorTest, RunLocalSelfInitError) {
    Scanner scanner("var a = \"outer\";\n{\n  var a = a;\n}");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    EXPECT_TRUE(resolver.has_errors());
}

TEST(EvaluatorTest, RunGlobalRedeclaration) {
    Scanner scanner("var a = \"1\";\nvar a = \"2\";\nprint a;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    EXPECT_FALSE(resolver.has_errors());
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "2\n");
}

TEST(EvaluatorTest, RunLocalRedeclarationError) {
    Scanner scanner("{\n  var a = \"1\";\n  var a = \"2\";\n}");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    EXPECT_TRUE(resolver.has_errors());
}

TEST(EvaluatorTest, RunClassInstance) {
    Scanner scanner("class Spaceship {}\nvar falcon = Spaceship();\nprint falcon;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Spaceship instance\n");
}

TEST(EvaluatorTest, RunInstanceProperty) {
    Scanner scanner("class Ship {}\nvar s = Ship();\ns.name = \"Falcon\";\nprint s.name;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Falcon\n");
}

TEST(EvaluatorTest, RunInstanceMethod) {
    Scanner scanner("class Robot {\n  beep() {\n    print \"Beep!\";\n  }\n}\nRobot().beep();");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Beep!\n");
}

TEST(EvaluatorTest, RunConstructor) {
    Scanner scanner("class Default {\n  init() {\n    this.x = \"bar\";\n  }\n}\nprint Default().x;");
    auto tokens = scanner.scan_tokens();
    Parser parser(std::move(tokens));
    auto statements = parser.parse_statements();
    Resolver resolver;
    resolver.resolve(statements);
    testing::internal::CaptureStdout();
    interpret(statements);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "bar\n");
}
