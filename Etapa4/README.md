# Stage 3 - Abstract Syntax Tree (AST)

## Abstract Syntax Tree (AST)

An Abstract Syntax Tree (AST) is a tree representation of the syntactic structure of source code. Each node in the tree represents a construct occurring in the source code, such as expressions, statements, or declarations. The AST abstracts away syntactic details like parentheses or semicolons, focusing on the hierarchical structure of the code.

### Construction of the AST

The AST is constructed during the parsing phase of compilation. The parser processes the source code according to the grammar rules defined in `parser.ypp` and generates the corresponding tree structure. The nodes of the AST are implemented in `ast.hpp` and `ast.cpp`, where each type of node corresponds to a specific language construct.

### Decompilation of the AST

Decompilation involves traversing the AST to regenerate source code that is semantically equivalent to the original. This process ensures that the structure and meaning of the code are preserved. The decompiled code is written to the output file specified during execution, allowing verification of the correctness of the AST and its transformations.

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
./etapa3 input.txt output.txt
```

Where:

- `input.txt` is the file with 2025++1 source code. A sample input file is provided in the repository, named `etapa3.txt`.
- `output.txt` is the file where the decompiled code will be written

## Decompilation Verification

To test if the decompilation is correct:

```bash
make test
```

This command compiles the original code, then compiles the decompiled code and verifies if both produce equivalent ASTs.

## Cleaning the Project

To clean the project and remove generated files, use:

```bash
make clean
```

This command will remove all object files, the executable, and any other generated files, allowing for a fresh build.

## File Structure

- `scanner.l`: Lexical analyzer specification
- `parser.ypp`: Syntactic analyzer grammar
- `ast.hpp` and `ast.cpp`: Abstract Syntax Tree implementation
- `symbol.hpp` and `symbol.cpp`: Symbol table management
- `main.cpp`: Program entry point
- `Makefile`: Compilation instructions
- `spect3.pdf`: PDF with the specification of this stage, in portuguese
