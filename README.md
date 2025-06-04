# The Descent Continues

This project implements a simple arithmetic expression evaluator in C++ using a custom-built 
lexer and parser. It reads input like `2 + 2`, tokenizes it, builds an abstract syntax tree 
(AST), and evaluates the result.

## Features

- **Lexer**: Breaks down input strings into tokens (numbers, operators, etc.)
- **Parser**: Constructs an AST from the tokens
- **Evaluator**: Traverses the AST to compute the result
- **AST Visualization**: Outputs tree structure of parsed expressions (optional)
- **Memory Debugging**: Tested and debugged using Valgrind

## Example Input

```
TESTING: 2+2
Eval parser result: 4
AST: (2+2)
```

## Technologies Used

- C++
- Regular expressions (`<regex>`)
- Abstract Syntax Trees (ASTs)
- Manual memory management
- Valgrind (for memory debugging)

## How to Build and Run

```bash
g++ -std=c++17 -Wall -Wextra -Werror -o descent main.cpp
./descent
```

## Educational Context

This project was developed as part of coursework to explore fundamental compiler components like 
lexical analysis, parsing, and AST evaluation.

---

## Author

Tyler Franklin  
[LinkedIn](https://www.linkedin.com/in/tyler-franklin-84aa0b144)

