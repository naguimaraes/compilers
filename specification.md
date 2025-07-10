# 2025++1 Language Specification

2025++1 is a simplified programming language inspired by C, designed specifically for educational purposes in compiler construction. The language provides fundamental programming features while remaining straightforward enough to implement a full compiler within the scope of a university course.

## Notable Language Features

- The language has a reversed internal representation for numerical literals.
- Variables must be declared with a type before use and its initialization is mandatory.
- Negative numbers can be read but can't be declared.
- No pointers or memory management.
- Only two data types are compatible: `int` and `byte`.
- Limited scoping rules (closer to traditional C).
- Programs must have a `main()` function as the entry point.

## Data Types

- **byte**: stores single characters (e.g., `'a'`, `'b'`, `'\n'`)
- **int**: Integer data type for general numeric values (e.g., `123`, `-456`).

- **real**: Floating-point data type, represented by a fraction (e.g., `5/7`)
- **bool**: Boolean data type, can be `TRUE` or `FALSE`.

## Variables and Constants

- Literals supported:
  - Character literals (e.g., `'a'`, `'\n'`)
  - String literals (e.g., `"Hello, World!"`)
  - Real literals (e.g., `3/4`, `1/2`)
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

    if (condition) {
        // statements
    }
    ```

- Loops:

    ```c
    while (expression) do {
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
    int add(int a, int b) {
        return a + b;
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