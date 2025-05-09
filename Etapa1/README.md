# Stage 1 - Lexical Analysis

## Lexical Analysis

Lexical Analysis is the first phase of a compiler that converts a sequence of characters from the source code into a sequence of tokens. These tokens are the vocabulary of the language and represent basic syntactic elements like keywords, identifiers, and literals.

### Implementation of the Lexical Analyzer

The lexical analyzer is implemented using Flex (scanner.l), which generates the C++ code for the scanner. It recognizes patterns in the input text and produces tokens according to predefined rules. The tokens are defined in `tokens.h` and are used by subsequent phases of the compilation process.

### Symbol Table

The symbol table is an essential data structure in the compilation process. It stores information about identifiers (such as variable names) and literals encountered in the source code. This information includes the lexeme (the actual text) and its type. The symbol table is implemented in `symbol.cpp` and `symbol.h`.

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
./etapa1 input.txt
```

Where:

- `input.txt` is the file with 2025++1 source code to be analyzed. A sample input file is provided in the repository, named `test.txt`.

## Output

The output of the execution will indicate whether the source code adheres to the lexical rules of the language. If the code is lexically correct, the program will terminate successfully, along with printing both the total line count and the whole symbol table. If there are lexical errors, an error message will be displayed, along with the line number where it occurred.

## Cleaning the Project

To clean the project and remove generated files, use:

```bash
make clean
```

This command will remove all object files, the executable, and any other generated files, allowing for a fresh build.

## File Structure

- `scanner.l`: Lexical analyzer specification with regular expressions for token recognition
- `tokens.h`: Definition of token types
- `symbol.cpp` and `symbol.h`: Symbol table implementation
- `main.cpp`: Program entry point and control flow
- `Makefile`: Compilation instructions