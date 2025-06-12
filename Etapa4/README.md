# Stage 4 - Semantic Verification

## Semantic Verification

Semantic verification is the phase of compilation that ensures the code adheres to the language's semantic rules. While syntax analysis confirms the code follows grammatical rules, semantic verification ensures operations are meaningful and consistent. This stage checks for issues like type compatibility, undeclared variables, and redeclarations.

### Implementation of Semantic Verification

The semantic analyzer, implemented in `verifications.cpp` and `verifications.hpp`, performs several checks on the Abstract Syntax Tree (AST) constructed during the parsing phase:

1. **Variable and Function Declarations**: Ensures all identifiers are properly declared before use and detects redeclarations.
2. **Type Checking**: Verifies type compatibility in expressions, assignments, and function calls.
3. **Vector Access Validation**: Checks that vector indices are of appropriate types.
4. **Function Return Types**: Ensures return statements match the declared function return type.
5. **Function Parameter Consistency**: Validates that function calls provide the correct number and types of arguments.

### Error Reporting

When semantic errors are detected, detailed error messages are generated that include:

- Line number where the error occurred
- Description of the error
- Contextual information (e.g., variable names, expected vs. actual types)

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

The compiler will perform both syntactic analysis and semantic verification. If semantic errors are detected, detailed error messages will be displayed, and the program will exit with code 4.

## Semantic Analysis Testing

To test both the decompilation and semantic verification:

```bash
make test
```

This command compiles the original code, verifies its semantics, then compiles the decompiled code and checks if both produce equivalent ASTs and semantic verification results.

## Error Cases

The semantic analyzer can detect and report various types of errors:

1. **Type Mismatches**: When expressions or assignments involve incompatible types.
2. **Undeclared Identifiers**: When variables, vectors, or functions are used without declaration.
3. **Redeclarations**: When an identifier is declared multiple times in the same scope.
4. **Invalid Vector Operations**: Improper index types or out-of-bounds access.
5. **Function Call Errors**: Wrong number of arguments or type mismatches.
6. **Return Statement Errors**: Missing return statements or type mismatches.

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
- `symbol.hpp` and `symbol.cpp`: Symbol table management with enhanced type support
- `verifications.hpp` and `verifications.cpp`: Semantic verification implementation
- `main.cpp`: Program entry point with semantic verification integration
- `Makefile`: Compilation instructions
- `spect4.pdf`: PDF with the specification of this stage, in Portuguese
