# Stage 6 - Target Code Generation

## Description

This stage implements x86 assembly code generation from TAC (Three-Address Code) instructions generated in Stage 5. The generator translates each type of TAC instruction into equivalent sequences of x86 assembly instructions.

## Main Files

- `asm.hpp` - Assembly generator interface
- `asm.cpp` - Assembly generator implementation
- `main.cpp` - Main program integrated with assembly generation
- `Makefile` - Updated to include assembly generator compilation

## How to Use

### Compilation

To compile the project, run:

```bash
make
```

or

```bash
make etapa6
```

To compile both the project and the assembly code generated, run:

```bash
make assemble
```

To easily run the compiler and generate assembly code, you can use:

```bash
make run
```

When running with `make run` or `make assemble`, it will compile all the source codes and generate in the `output/` directory all the intermediate data structures involved in the compilation process:

- Symbol table in `output/etapa6_symbol_table.txt`
- AST structure in `output/etapa6_AST.txt`
- Decompiled code from AST in `output/etapa6_decompiled.txt`
- TAC instructions in `output/etapa6_TAC.txt`
- Generated assembly file in `output/etapa6.s`
- Executable file in `output/etapa6.out` (if compiled with `gcc`)

### Execution

```bash
./etapa6 <input_file> <symbol_table_output> <ast_output> <decompiled_output> <tac_output> <assembly_output>
```

**Parameters:**

- `input_file`: Source file in 2025++1 language
- `symbol_table_output`: Symbol table output file
- `ast_output`: AST structure output file
- `decompiled_output`: Decompiled code from AST
- `tac_output`: TAC instructions output file
- `assembly_output`: Generated assembly file

### Usage Example

```bash
# Compile complete example
./etapa6 etapa6.2025++1 output/etapa6_symbol_table.txt output/etapa6_AST.txt output/etapa6_decompiled.txt output/etapa6_TAC.txt output/etapa6.s

# Compile assembly to executable
gcc -m32 output/etapa6.s -o output/etapa6.out
```

## Code Architecture

### AssemblyGenerator Functions

- **generateASM()**: Main function that processes TAC list
- **processInstruction()**: Handles specific TAC instruction types
- **Helper functions**: Utility functions for allocation and manipulation
- **Type detection**: Identifies real vs integer variables for proper handling

## Implemented Features

### Supported TAC Types

1. **Variable Initialization (INIT)**
   - Allocates space on stack and initializes with value
   - Support for both integer and real variables
   - Example: `int a = 10;` → `movl $10, -4(%ebp)`
   - Example: `real r = 7/10;` → `r: .float 0.7`

2. **Assignments (MOVE)**
   - Moves values between variables and registers
   - Example: `a = b;` → `movl -8(%ebp), %eax; movl %eax, -4(%ebp)`

3. **Arithmetic Operations**
   - **ADD**: `addl src, dest` (integers) / `fadds src` (reals)
   - **SUB**: `subl src, dest` (integers) / `fsubs src` (reals)
   - **MUL**: `imull src, dest` (integers) / `fmuls src` (reals)
   - **DIV**: `idivl divisor` (integers) / `fdivs src` (reals)
   - **MOD**: `idivl divisor` (remainder in %edx)

4. **Comparison Operations**
   - **LT, GT, LE, GE, EQ, NE**: Generate code with comparison and conditional jumps
   - Result in temporary variable (0 = false, 1 = true)

5. **Logical Operations**
   - **AND**: `andl src, dest`
   - **OR**: `orl src, dest`
   - **NOT**: Logical inversion with zero comparison

6. **Control Flow**
   - **LABEL**: Code markers
   - **JUMP**: Unconditional jump `jmp label`
   - **IFZ**: Conditional jump if zero `je label`

7. **Functions**
   - **BEGINFUN**: Function prologue (stack frame setup)
   - **ENDFUN**: Function epilogue (stack frame restoration)
   - **CALL**: Function call `call function_name`
   - **ARG**: Argument placement on stack `pushl arg`
   - **RET**: Function return `movl value, %eax`

8. **Input/Output**
   - **PRINT**: `printf` call with appropriate format
     - Integers: `"%d"` format
     - Reals: `"%.2f"` format
     - Characters: `"%c"` format
     - Strings: `"%s"` format
   - **READ**: `scanf` call for input

9. **Vectors**
   - **VECWRITE**: Vector writing using calculated addressing
   - **VECREAD**: Vector reading using calculated addressing
   - **BEGINVEC/ENDVEC**: Vector declaration delimiters

10. **Real Number Support**
    - **Data section**: `.float` declarations for real variables
    - **FPU operations**: Uses x87 floating-point unit instructions
    - **Real arithmetic**: `fadds`, `fsubs`, `fmuls`, `fdivs`
    - **Real printing**: FPU stack manipulation for printf
    - **Fraction evaluation**: Automatic evaluation of expressions like `7/10` → `0.7`

## Generator Characteristics

### Memory Management

- **Stack-based**: All variables are allocated on the stack
- **Global variables**: Real and integer variables in `.data` section
- **Automatic offset**: Automatically calculates offsets from %ebp
- **Mapping**: Maintains variable location table

### Register Conventions

- **%eax**: Main register for operations and return values
- **%ebx**: Auxiliary register for binary operations
- **%ecx, %edx**: Used as needed (division uses %edx)
- **%ebp**: Base pointer for stack frame
- **%esp**: Stack pointer
- **FPU stack**: For floating-point operations

### Output Format

```assembly
.section .data
    int_format: .string "%d"
    char_format: .string "%c"
    real_format: .string "%.2f"
    string_format: .string "%s"
    scanf_int: .string "%d"
    scanf_real: .string "%f"
    # Global variables
    variable_name: .long value        # for integers
    real_variable: .float value       # for reals

.section .text
.globl main
.extern printf
.extern scanf

main:
    pushl %ebp
    movl %esp, %ebp
    # ... function code
    movl %ebp, %esp
    popl %ebp
    ret
```

## File Structure

- `scanner.l`: Lexical analyzer specification
- `parser.ypp`: Syntactic analyzer grammar
- `ast.hpp` and `ast.cpp`: Abstract Syntax Tree implementation
- `symbol.hpp` and `symbol.cpp`: Symbol table management
- `verifications.hpp` and `verifications.cpp`: Semantic analysis implementation
- `tac.hpp` and `tac.cpp`: Three Address Code generation implementation
- `asm.hpp` and `asm.cpp`: Assembly code generation implementation
- `main.cpp`: Program entry point
- `Makefile`: Compilation instructions
- `spect6.pdf`: PDF with the specification of this stage, in portuguese
- `etapa6.2025++1`: Sample input file with 2025++1 source code
