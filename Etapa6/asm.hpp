// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Assembly (ASM) generator header file made by Nathan Alonso Guimarães (00334437)

#ifndef ASM_HPP
#define ASM_HPP

#include <string>

class TAC;

void generateASM(TAC* tacHead, const std::string& outputFileName);

#endif // ASM_HPP