// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Assembly (ASM) generator source code made by Nathan Alonso Guimarães (00334437)

#include "asm.hpp"
#include "tac.hpp"
#include "symbol.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <vector>

// Global state variables
std::ofstream* outFile = nullptr;                                    // Output file stream
std::unordered_map<std::string, std::string> varLocations;          // Variable to memory location mapping
std::unordered_map<std::string, int> vectorSizes;                   // Vector name to size mapping
std::vector<std::string> stringLiterals;                            // String literals storage
int stackOffset = 0;                                                // Current stack offset for local variables
int labelCounter = 0;                                               // Counter for generating unique labels
int stringCounter = 0;                                              // Counter for string literals
std::string currentRegister = "%eax";                               // Currently used register (usually %eax)

// Utility functions
std::string getOperandLocation(class Symbol* symbol);
std::string getOperandValue(class Symbol* symbol);
std::string allocateVariable(const std::string& name, int size = 4);
std::string allocateVector(const std::string& name, int elements);
std::string getStringLabel(const std::string& str);
void emitInstruction(const std::string& instruction);
void emitLabel(const std::string& label);
void emitComment(const std::string& comment);


std::string getOperandLocation(Symbol* symbol) {
    if (!symbol) return "";
    
    std::string name = symbol->getLexeme();
    
    // Check if already allocated
    auto it = varLocations.find(name);
    if (it != varLocations.end()) {
        return it->second;
    }
    
    // Allocate new location
    return allocateVariable(name);
}

std::string getOperandValue(Symbol* symbol) {
    if (!symbol) return "";
    
    std::string text = symbol->getLexeme();
    
    // Check if it's a numeric literal
    if (text[0] == '\'' && text.length() == 3 && text[2] == '\'') {
        // Character literal 'x' -> ASCII value
        return "$" + std::to_string((int)text[1]);
    }
    
    if (std::isdigit(text[0]) || (text[0] == '-' && text.length() > 1)) {
        // Numeric literal
        return "$" + text;
    }
    
    if (text[0] == '"') {
        // String literal - return as is for now
        return text;
    }
    
    // Variable reference
    return getOperandLocation(symbol);
}

std::string allocateVariable(const std::string& name, int size) {
    auto it = varLocations.find(name);
    if (it != varLocations.end()) {
        return it->second;
    }
    
    stackOffset += size;
    std::string location = "-" + std::to_string(stackOffset) + "(%ebp)";
    varLocations[name] = location;
    return location;
}

std::string allocateVector(const std::string& name, int elements) {
    return allocateVariable(name, elements * 4);  // 4 bytes per element
}

std::string getStringLabel(const std::string& str) {
    // Find existing string or add new one
    auto it = std::find(stringLiterals.begin(), stringLiterals.end(), str);
    if (it != stringLiterals.end()) {
        size_t index = std::distance(stringLiterals.begin(), it);
        return "$str_" + std::to_string(index);
    }
    
    // Add new string
    stringLiterals.push_back(str);
    return "$str_" + std::to_string(stringLiterals.size() - 1);
}

void emitInstruction(const std::string& instruction) {
    (*outFile) << "\t" << instruction << std::endl;
}

void emitLabel(const std::string& label) {
    (*outFile) << label << std::endl;
}

void emitComment(const std::string& comment) {
    (*outFile) << "\t# " << comment << std::endl;
}

// Public interface function
void generateASM(TAC* tacHead, const std::string& outputFileName) {
    std::ofstream file(outputFileName);
    if (!file) {
        std::cerr << "Error: Could not open output file " << outputFileName << std::endl;
        return;
    }
    
    // Initialize global state
    outFile = &file;
    varLocations.clear();
    vectorSizes.clear();
    stringLiterals.clear();
    stackOffset = 0;
    labelCounter = 0;
    stringCounter = 0;
    currentRegister = "%eax";
    
    // Check if TAC exists
    if (!tacHead) {
        file.close();
        return;
    }
    
    emitComment("Generated x86 Assembly Code from TAC");
    emitComment("Compiled with 2025++1 Compiler");
    (*outFile) << std::endl;
    
    // Process in the same order as printTAC function
    // Start from the end and work backwards
    TAC* current = tacHead;
    
    // Collect vector information first
    while (current) {
        if (current->getType() == TACType::BEGINVEC) {
            std::string vecName = current->getOp1()->getLexeme();
            int size = std::stoi(current->getOp2()->getLexeme());
            vectorSizes[vecName] = size;
        }
        current = current->getPrev();
    }
    
    // Generate data section
    emitLabel(".section .data");
    emitInstruction("int_format: .string \"%d\"");
    emitInstruction("char_format: .string \"%c\"");
    emitInstruction("real_format: .string \"%.2f\"");
    emitInstruction("string_format: .string \"%s\"");
    emitInstruction("newline: .string \"\\n\"");
    emitInstruction("scanf_int: .string \"%d\"");
    emitInstruction("scanf_char: .string \" %c\"");
    emitInstruction("scanf_real: .string \"%f\"");
    
    // String literals
    for (size_t i = 0; i < stringLiterals.size(); i++) {
        std::string label = "str_" + std::to_string(i);
        emitInstruction(label + ": .string " + stringLiterals[i]);
    }
    (*outFile) << std::endl;
    
    // Generate text section
    emitLabel(".section .text");
    emitLabel(".globl _start");
    emitLabel("_start:");
    emitInstruction("call main");
    emitInstruction("movl %eax, %ebx");  // Exit code
    emitInstruction("movl $1, %eax");    // sys_exit
    emitInstruction("int $0x80");        // System call
    emitLabel(".extern printf");
    emitLabel(".extern scanf");
    (*outFile) << std::endl;
    
    // Process TAC instructions in the same order as printTAC
    current = tacHead;
    while (current) {
        switch (current->getType()) {
            case TACType::INIT: {
                std::string varName = current->getRes()->getLexeme();
                std::string value = getOperandValue(current->getOp1());
                std::string location = allocateVariable(varName);
                emitComment("Initialize " + varName + " = " + value);
                emitInstruction("movl " + value + ", " + location);
                break;
            }
            case TACType::MOVE: {
                std::string dest = current->getRes()->getLexeme();
                std::string src = getOperandValue(current->getOp1());
                std::string destLoc = getOperandLocation(current->getRes());
                emitComment("Move " + src + " to " + dest);
                if (src[0] == '$') {
                    emitInstruction("movl " + src + ", " + destLoc);
                } else {
                    emitInstruction("movl " + src + ", %eax");
                    emitInstruction("movl %eax, " + destLoc);
                }
                break;
            }
            case TACType::PRINT: {
                std::string value = getOperandValue(current->getOp1());
                emitComment("Print " + value);
                std::string format = "$int_format";
                if (value.front() == '"' && value.back() == '"') {
                    format = "$string_format";
                    value = getStringLabel(value);
                }
                if (value[0] == '$') {
                    emitInstruction("pushl " + value);
                } else {
                    emitInstruction("pushl " + value);
                }
                emitInstruction("pushl " + format);
                emitInstruction("call printf");
                emitInstruction("addl $8, %esp");
                break;
            }
            case TACType::BEGINFUN: {
                std::string funcName = current->getOp1()->getLexeme();
                emitLabel(funcName + ":");
                emitComment("Function " + funcName + " prologue");
                emitInstruction("pushl %ebp");
                emitInstruction("movl %esp, %ebp");
                stackOffset = 0;
                break;
            }
            case TACType::ENDFUN: {
                std::string funcName = current->getOp1()->getLexeme();
                emitComment("Function " + funcName + " epilogue");
                emitInstruction("movl %ebp, %esp");
                emitInstruction("popl %ebp");
                emitInstruction("ret");
                (*outFile) << std::endl;
                break;
            }
            case TACType::RET: {
                if (current->getOp1()) {
                    std::string retVal = getOperandValue(current->getOp1());
                    emitComment("Return " + retVal);
                    if (retVal[0] == '$') {
                        emitInstruction("movl " + retVal + ", %eax");
                    } else {
                        emitInstruction("movl " + retVal + ", %eax");
                    }
                }
                break;
            }
            default:
                emitComment("Unhandled TAC instruction");
                break;
        }
        current = current->getPrev();
    }
    
    file.close();
}
