# 2025++1 Language Specification

2025++1 is a simplified programming language inspired by C, designed specifically for educational purposes in compiler construction. The language provides fundamental programming features while remaining straightforward enough to implement a full compiler within the scope of a university course.

## Data Types

- **byte**: 8-bit integer data type (for small numeric values)
- **int**: Integer data type for general numeric values
- **real**: Floating-point data type, represented by a fraction (e.g., `5/7`)

## Variables and Constants

- Variables must be declared with a type before use.
- Literals supported:
- Integer literals (e.g., `123`, `001`)

## Arrays

- One-dimensional arrays: `int v[10];`
- Arrays can be initialized with comma-separated literals: `int v[10] = 'a', 4, 'b', 500, 10, 0, 0, 0, 0, 0;`
- Arrays are accessed with square brackets: `v[7] = 55;`

## Operators

- Arithmetic: `+`, `-`, `*`, `/`, `%` (modulo)
- Relational: `<`, `>`, `<=`, `>=`, `==` (equality), `!=` (inequality)
- Logical: `&` (AND), `|` (OR), `~` (NOT)
- Assignment: `=`

## Control Flow

- Conditional statements:

    ```c
    if (condition) {
            // statements
    } else {
            // statements
    }
    ```

- Loops:

    ```c
    while (expression) {
            // statements
    }

    do {
            // statements
    } while (expression);
    ```

## Functions

- Functions must be declared with return type, name, and parameters.
- Example:

    ```c
    int incn(int p, int n) {
            return p + n;
    }
    ```

- The `return` statement returns a value from a function:

    ```c
    return expression;
    ```

- Function calls: `x = incn(x, 1);`

## Input/Output

- `read`: Reads a value from standard input into a variable.

    ```c
    read number;
    ```

- `print`: Outputs values or strings to standard output.

    ```c
    print "Result: " x "\n";
    ```

## Comments

- Single-line comments: `// This is a comment`
- Multi-line comments:

    ```c
    /--
    This is a multi-line
    comment
    --/
    ```

## Notable Language Features

- No pointers or memory management.
- No type conversion (each operation requires compatible types).
- Limited scoping rules (closer to traditional C).
- The language has a reversed internal representation for numerical literals.
- Programs must have a `main()` function as the entry point.
