# Development of a compiler from scratch

This project implements a compiler for a simplified language named "2025++1", developed as a practical and educational project for the Compilers course (INF01147) at Federal University of Rio Grande do Sul (UFRGS).

## Project Structure

The compiler is being developed in progressive stages, where each stage adds new functionalities. Inside each stage folder, you will find the source code and documentation related to that specific stage.
The stages are as follows:

### [Stage 1 (Etapa1)](./Etapa1): Lexical Analysis and Symbol Table ✅

- Implementation of the lexical analyzer using Flex
- Recognition of tokens (keywords, identifiers, literals)
- Lexical error handling
- Symbol table construction

### [Stage 2 (Etapa2)](./Etapa2): Syntactic Analysis ✅

- Implementation of the syntactic analyzer using Bison
- Formal grammar for the 2025++1 language
- Program structure recognition
- Syntactic error handling

### [Stage 3 (Etapa3)](./Etapa3): Abstract Syntax Tree (AST) and Decompilation ✅

- Construction of the Abstract Syntax Tree
- Hierarchical representation of the program structure
- Decompilation (source code generation from the AST)
- Verification of equivalence between original code and decompiled code

### [Stage 4 (Etapa4)](./Etapa4): Semantic Verification ⏳

## 2025++1 Language Specification

A detailed specification of the 2025++1 language can be found in the [specification.md](./specification.md) file. This document outlines the syntax, semantics, and features of the language.
