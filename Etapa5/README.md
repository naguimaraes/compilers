# Stage 5 - Intermediate Code Generation (TAC)

## Three Address Code (TAC)

Three Address Code (TAC) is an intermediate representation of the source code that simplifies the structure of expressions and statements. Each TAC instruction has at most three operands: two source operands and one destination operand. This representation is closer to assembly language but still maintains some abstraction from the target machine architecture.

### TAC Generation

The TAC generation process traverses the Abstract Syntax Tree (AST) and translates high-level language constructs into a sequence of simple three-address instructions. The implementation is found in `tac.hpp` and `tac.cpp`, where each type of AST node is translated to its corresponding TAC instructions.

#### TAC Instruction Types

The compiler supports the following TAC instruction types:

- **Arithmetic Operations**: ADD, SUB, MUL, DIV, MOD
- **Logical Operations**: AND, OR, NOT
- **Comparison Operations**: LT, GT, LE, GE, EQ, NE
- **Assignment Operations**: MOVE, INIT
- **Control Flow**: LABEL, IFZ (conditional jump), JUMP (unconditional jump)
- **Function Operations**: BEGINFUN, ENDFUN, CALL, ARG, RET
- **I/O Operations**: PRINT, READ
- **Vector Operations**: VECREAD, VECWRITE, BEGINVEC, ENDVEC

### Temporary Variables and Labels

During code generation, the compiler automatically creates:

- **Temporary variables** (e.g., `temp0`, `temp1`) to store intermediate results
- **Labels** (e.g., `label0`, `label1`) for control flow instructions

### TAC Output Format

The generated TAC is displayed in a table format showing:

- Instruction type
- Result operand
- First operand  
- Second operand

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
./etapa5 input.txt output.txt
```

Where:

- `input.txt` is the file with 2025++1 source code. A sample input file is provided in the repository, named `etapa5.txt`.
- `output.txt` is the file where the decompiled code will be written

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
- `verifications.hpp` and `verifications.cpp`: Semantic analysis implementation
- `tac.hpp` and `tac.cpp`: Three Address Code generation implementation
- `main.cpp`: Program entry point
- `Makefile`: Compilation instructions
- `spect5.pdf`: PDF with the specification of this stage, in portuguese

## TAC Generation Features

This stage implements comprehensive TAC generation for:

- **Variable declarations and assignments**
- **Arithmetic and logical expressions**
- **Control flow structures** (if statements, while loops, do-while loops)
- **Function declarations and calls**
- **Vector operations** (declaration, access, assignment)
- **I/O operations** (print and read statements)
- **Type-aware code generation** with proper temporary variable management

The generated TAC provides a solid foundation for further optimization phases and target code generation in subsequent compiler stages.
