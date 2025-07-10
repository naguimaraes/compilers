// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// TAC optimizer header file made by Nathan Alonso Guimarães (00334437)

#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <string>

class TAC;

TAC* optimizeTAC(TAC* tacHead, const std::string& outputFileName);

#endif // OPTIMIZER_HPP