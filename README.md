# LoxInterp

C++23 tree-walk interpreter for [Lox](https://craftinginterpreters.com/the-lox-language.html), following [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom.

## Quick Start

**Requirements**: C++23 compiler (GCC 13+), CMake 3.21+, Ninja, GTest (for tests).

```sh
git clone <this-repo>
cd LoxInterp
./build.sh              # Debug build
./run_tests.sh          # Run 127 tests
```

**Dependencies**: zero. GTest is fetched via system package at build time.

## Commands

```sh
./build/LoxInterp tokenize <file>   # Lexical analysis
./build/LoxInterp parse <file>      # Parse to AST (s-expression)
./build/LoxInterp evaluate <file>   # Evaluate single expression
./build/LoxInterp run <file>        # Execute full program
```

## Architecture

```
                    Source Code
                         │
          ┌──────────────┼──────────────┐
          ▼              ▼              ▼
      tokenize         parse        evaluate / run
          │              │              │
          ▼              ▼              ▼
    ┌─────────┐    ┌─────────┐    ┌──────────────┐
    │ Scanner │───▶│ Parser  │───▶│  Interpreter  │
    └─────────┘    └─────────┘    └──────────────┘
         │              │              │    ▲
         ▼              ▼              │    │
     Token流         AST树         Runtime│   │
                        │           Value│   │
                        ▼              │    │
                   ┌──────────┐        │    │
                   │ Resolver │────────┘    │
                   └──────────┘ annotates AST
                        │    with depths
                        ▼
              Compile-time errors (65)
```

```
src/
├── main.cpp              # CLI entry point
├── scanner/              # Lexer: source → tokens (39 token types)
├── ast/                  # AST node definitions (7 Expr + 6 Stmt = 13 types)
├── parser/               # Recursive-descent: tokens → AST + print_ast
├── resolver/             # Compile-time binding, scope analysis, validation
├── interpreter/          # Tree-walk: evaluate expressions, execute statements
└── util/                 # overloaded helper, number formatting
```

**Exit codes**: 65 = lexical/syntax/resolver error, 70 = runtime error.

## Language Features

- **Literals**: `true`, `false`, `nil`, numbers, strings
- **Operators**: `+ - * / < <= > >= == != ! = and or`
- **Control flow**: `if/else`, `while`, `for` (desugared to while)
- **Functions**: `fun name(params) { body }`, closures, `return`, native `clock()`
- **Classes**: `class Name { methods }`, instances, properties, methods, `this`, `init` constructor, `class Sub < Super` inheritance, `super.method()`

## Example

Input `fibonacci.lox`:

```lox
fun fib(n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

var start = clock();
print "fib(10) = " + fib(10);
print "time: " + (clock() - start) + "s";
```

Output:

```
fib(10) = 55
time: 0s
```

---

Input `classes.lox`:

```lox
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
var d = Dog("Buddy");
d.speak();
```

Output:

```
Buddy
woof!
```
