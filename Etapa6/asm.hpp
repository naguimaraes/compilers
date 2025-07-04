// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Assembly (ASM) generator header file made by Nathan Alonso Guimarães (00334437)

#ifndef ASM_HPP
#define ASM_HPP

#include <string>

// Forward declarations
class TAC;

/**
 * @brief Generate x86 assembly code from Three-Address Code (TAC)
 * 
 * This function translates a list of TAC instructions into equivalent x86 assembly code.
 * It handles all TAC instruction types including arithmetic operations, control flow,
 * function calls, I/O operations, and vector operations.
 * 
 * @param tacHead Pointer to the first TAC instruction in the list
 * @param outputFileName Name of the output assembly file
 */
void generateASM(TAC* tacHead, const std::string& outputFileName);

#endif // ASM_HPP