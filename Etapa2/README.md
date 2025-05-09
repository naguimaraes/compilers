# Stage 2 - Syntactic Analysis

## Syntactic Analysis

Syntactic Analysis, also known as parsing, is the second phase of a compiler that analyzes the structure of the source code according to the grammar rules of the programming language. It verifies that the tokens produced by the lexical analyzer form valid language constructs.

### Implementation of the Syntactic Analyzer

The syntactic analyzer is implemented using Bison (parser.ypp), which generates the C++ code for the parser. It defines the grammar rules of the 2025++1 language and constructs a parse tree representing the structure of the input program. The parser works in conjunction with the lexical analyzer to interpret the source code.

### Syntax Error Handling

The parser includes mechanisms for detecting and reporting syntax errors in the source code. When an error is detected, the parser provides descriptive error messages indicating the type of error and the line number where it occurred. This helps developers to quickly identify and fix issues in their code.

## Project Compilation and Execution

To compile the project:

```bash
make
```

To run the compiler:

```bash
make run
```

or

```bash
./etapa2 input.txt
```

Where:

- `input.txt` is the file with 2025++1 source code to be analyzed. A sample input file is provided in the repository, named `etapa2.txt`.

## Output

The output of the execution will indicate whether the source code adheres to the grammar rules of the language. If the code is syntactically correct, the program will terminate successfully. If there are syntax errors, an error message will be displayed, along with the line number where it occurred.

## Cleaning the Project

To clean the project and remove generated files, use:

```bash
make clean
```

This command will remove all object files, the executable, and any other generated files, allowing for a fresh build.

## File Structure

- `scanner.l`: Lexical analyzer specification inherited from Stage 1
- `parser.ypp`: Grammar rules for the 2025++1 language
- `symbol.cpp` and `symbol.h`: Symbol table management
- `main.cpp`: Program entry point and control flow
- `Makefile`: Compilation instructions