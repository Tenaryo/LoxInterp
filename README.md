# LoxInterp

A C++23 tree-walk interpreter for the [Lox](https://craftinginterpreters.com/the-lox-language.html) language, following [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom.

## Architecture

```
src/
├── main.cpp              # CLI entry point
├── scanner/              # Lexer: source → tokens
│   ├── scanner.hpp
│   └── scanner.cpp
├── ast/                  # AST node definitions (Expr, Stmt)
│   └── ast.hpp
├── parser/               # Recursive-descent parser: tokens → AST
│   ├── parser.hpp
│   └── parser.cpp
├── resolver/             # Compile-time binding & validation
│   ├── resolver.hpp
│   └── resolver.cpp
├── interpreter/          # Tree-walk evaluator
│   ├── interpreter.hpp
│   └── interpreter.cpp
└── util/                 # Utilities
    ├── overloaded.hpp    # std::visit helper
    └── format.hpp        # Number formatting
```

## Supported Features

- **Scanner**: 39 token types, keywords, strings, numbers, comments, multi-line error reporting
- **Parser**: Full recursive-descent expression parser (assignment, logic, equality, comparison, arithmetic, unary, grouping)
- **AST**: 13 node types (7 Expr + 6 Stmt)
- **Interpreter**: tree-walk evaluation with environment chain for scoping
- **Resolver**: compile-time variable binding, self-init detection, redeclaration check, `this`/`super` validation
- **Statements**: print, expression, variable declaration, block, if/else, while, for (desugared)
- **Functions**: declarations, calls, closures, return statements, native `clock()`
- **Classes**: declarations, instances, properties (get/set), methods, `this`, constructors (`init`), inheritance, `super`
- **Error handling**: lexical (65), syntax (65), runtime (70), resolver (65)
- **Tests**: 127 tests across scanner, parser, interpreter

## Commands

```sh
./your_program.sh tokenize <file>   # Lexical analysis
./your_program.sh parse <file>      # Parse to AST (prints s-expression)
./your_program.sh evaluate <file>   # Evaluate single expression
./your_program.sh run <file>        # Execute full program
```

## Build & Test

```sh
./build.sh              # Debug build
./build.sh Release      # Release build
./run_tests.sh          # Run all tests
```

## Language Features

```lox
// Variables & scoping
var x = 42;
{ var x = "shadow"; print x; }

// Functions & closures
fun counter() {
    var i = 0;
    fun next() { i = i + 1; return i; }
    return next;
}

// Classes & inheritance
class Animal {
    init(name) { this.name = name; }
    speak() { print this.name; }
}
class Dog < Animal {
    speak() {
        super.speak();
        print "woof!";
    }
}
```
