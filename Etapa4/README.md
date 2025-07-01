# Stage 4 - Semantic Verification

## Semantic Analysis

Semantic analysis is the phase of compilation that verifies the semantic correctness of the source code. While syntactic analysis ensures that the code follows the grammar rules, semantic analysis ensures that the code makes logical sense according to the language's semantic rules.

### Semantic Verifications Implemented

The semantic verifier implemented in `verifications.hpp` and `verifications.cpp` performs the following checks:

#### Declaration and Usage Verification

- **Undeclared variables**: Ensures all variables are declared before use
- **Redeclaration detection**: Prevents multiple declarations of the same identifier in the same scope
- **Function declaration verification**: Validates function signatures and parameter lists

#### Type Checking

- **Type compatibility**: Verifies that operations are performed on compatible types
- **Assignment compatibility**: Ensures assignments respect type rules
- **Function return type validation**: Checks that return statements match declared function return types
- **Parameter type matching**: Validates that function calls use correct parameter types

#### Control Flow Verification

- **Return statement validation**: Ensures functions return appropriate values
- **Unreachable code detection**: Identifies code that can never be executed

### Error Reporting

The semantic analyzer provides comprehensive error reporting with:

- **Line number information**: Precise location of semantic errors
- **Descriptive error messages**: Clear explanation of what went wrong
- **Error classification**: Different types of semantic errors are clearly categorized

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
./etapa4 input.txt output.txt
```

Where:

- `input.txt` is the file with 2025++1 source code. A sample input file is provided in the repository, named `etapa4.txt`.
- `output.txt` is the file where the decompiled code will be written

## Semantic Verification Testing

To test if the semantic verification and decompilation work correctly:

```bash
make test
```

This command compiles the original code, then compiles the decompiled code and verifies if both produce equivalent ASTs through multiple recompilation cycles.

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
- `main.cpp`: Program entry point
- `Makefile`: Compilation instructions
- `spect4.pdf`: PDF with the specification of this stage, in portuguese

## Semantic Features

This stage implements comprehensive semantic verification for:

- **Variable and function declarations**
- **Type system enforcement**
- **Scope management and identifier resolution**
- **Function call validation**
- **Control flow analysis**
- **Error detection and reporting**

The semantic analyzer ensures that only semantically correct programs proceed to the code generation phase, providing a robust foundation for the subsequent compilation stages.