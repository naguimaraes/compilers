# Development of a compiler from scratch

This project implements a compiler for a simplified language named "2025++1", developed as a practical and educational project for the Compilers course (INF01147) at Federal University of Rio Grande do Sul (UFRGS).

## Project Structure

The compiler is being developed in progressive stages, where each stage adds new functionalities:

### [Stage 1 (Etapa1)](./Etapa1): Lexical Analysis

- Implementation of the lexical analyzer using Flex
- Recognition of tokens (keywords, identifiers, literals)
- Lexical error handling
- Symbol table construction

### [Stage 2 (Etapa2)](./Etapa2): Syntactic Analysis

- Implementation of the syntactic analyzer using Bison
- Formal grammar for the 2025++1 language
- Program structure recognition
- Syntactic error handling

### [Stage 3 (Etapa3)](./Etapa3): Abstract Syntax Tree (AST)

- Construction of the Abstract Syntax Tree
- Hierarchical representation of the program structure
- Decompilation (source code generation from the AST)
- Verification of equivalence between original code and decompiled code

## 2025++1 Language

2025++1 is a simplified version of C, supporting:

- Data types: `byte`, `int`, `real`
- Variable and array declarations
- Function declarations and calls
- Flow control commands: `if-else`, `while-do`, `do-while`
- Arithmetic and logical expressions
- Input and output commands: `read` and `print`

## Development History

- **Stage 1**: Complete lexical analysis ✅
- **Stage 2**: Complete syntactic analysis ✅
- **Stage 3**: Abstract Syntax Tree and Decompilation ✅
