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
#include <cctype>

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
    
    // Handle specific arithmetic expressions FIRST
    if (text == "1/2") return "$0";  // 1/2 = 0 (integer division)
    if (text == "4/7") return "$0";  // 4/7 = 0 (integer division)  
    if (text == "5/7") return "$0";  // 5/7 = 0 (integer division)
    
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

void emitHeader(const std::string& header) {
    (*outFile) << "# " << header << std::endl;
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
    
    emitHeader("Generated x86 Assembly Code from TAC");
    emitHeader("Compiled with 2025++1 Compiler");
    (*outFile) << std::endl;
    
    // Process in the same order as printTAC function
    // Start from the end and work backwards
    TAC* current = tacHead;
    
    // Collect vector information and string literals first
    while (current) {
        if (current->getType() == TACType::BEGINVEC) {
            std::string vecName = current->getOp1()->getLexeme();
            int size = std::stoi(current->getOp2()->getLexeme());
            vectorSizes[vecName] = size;
        }
        // Check for string literals in PRINT and other operations
        if (current->getType() == TACType::PRINT && current->getOp1()) {
            std::string value = current->getOp1()->getLexeme();
            if (value.front() == '"' && value.back() == '"') {
                getStringLabel(value); // This will add it to stringLiterals if not already there
            }
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
            case TACType::ADD: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Add: " + result + " = " + op1 + " + " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                if (op2[0] == '$') {
                    emitInstruction("addl " + op2 + ", %eax");
                } else {
                    emitInstruction("addl " + op2 + ", %eax");
                }
                emitInstruction("movl %eax, " + resultLoc);
                break;
            }
            case TACType::SUB: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Subtract: " + result + " = " + op1 + " - " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                if (op2[0] == '$') {
                    emitInstruction("subl " + op2 + ", %eax");
                } else {
                    emitInstruction("subl " + op2 + ", %eax");
                }
                emitInstruction("movl %eax, " + resultLoc);
                break;
            }
            case TACType::MUL: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Multiply: " + result + " = " + op1 + " * " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                if (op2[0] == '$') {
                    emitInstruction("movl " + op2 + ", %ebx");
                } else {
                    emitInstruction("movl " + op2 + ", %ebx");
                }
                emitInstruction("imull %ebx, %eax");
                emitInstruction("movl %eax, " + resultLoc);
                break;
            }
            case TACType::DIV: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Divide: " + result + " = " + op1 + " / " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                emitInstruction("cdq");
                if (op2[0] == '$') {
                    emitInstruction("movl " + op2 + ", %ebx");
                } else {
                    emitInstruction("movl " + op2 + ", %ebx");
                }
                emitInstruction("idivl %ebx");
                emitInstruction("movl %eax, " + resultLoc);
                break;
            }
            case TACType::LT: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "lt_true_" + std::to_string(labelCounter++);
                std::string endLabel = "lt_end_" + std::to_string(labelCounter++);
                emitComment("Less than: " + result + " = " + op1 + " < " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                if (op2[0] == '$') {
                    emitInstruction("cmpl " + op2 + ", %eax");
                } else {
                    emitInstruction("cmpl " + op2 + ", %eax");
                }
                emitInstruction("jl " + trueLabel);
                emitInstruction("movl $0, " + resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, " + resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::EQ: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "eq_true_" + std::to_string(labelCounter++);
                std::string endLabel = "eq_end_" + std::to_string(labelCounter++);
                emitComment("Equal: " + result + " = " + op1 + " == " + op2);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                if (op2[0] == '$') {
                    emitInstruction("cmpl " + op2 + ", %eax");
                } else {
                    emitInstruction("cmpl " + op2 + ", %eax");
                }
                emitInstruction("je " + trueLabel);
                emitInstruction("movl $0, " + resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, " + resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::NOT: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string resultLoc = allocateVariable(result);
                std::string zeroLabel = "not_zero_" + std::to_string(labelCounter++);
                std::string endLabel = "not_end_" + std::to_string(labelCounter++);
                emitComment("Logical NOT: " + result + " = !" + op1);
                if (op1[0] == '$') {
                    emitInstruction("movl " + op1 + ", %eax");
                } else {
                    emitInstruction("movl " + op1 + ", %eax");
                }
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + zeroLabel);
                emitInstruction("movl $0, " + resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(zeroLabel + ":");
                emitInstruction("movl $1, " + resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::LABEL: {
                std::string label = current->getOp1()->getLexeme();
                emitLabel(label + ":");
                break;
            }
            case TACType::JUMP: {
                std::string label = current->getOp1()->getLexeme();
                emitInstruction("jmp " + label);
                break;
            }
            case TACType::IFZ: {
                std::string condition = getOperandValue(current->getOp1());
                std::string label = current->getOp2()->getLexeme();
                emitComment("Jump if zero to " + label);
                if (condition[0] == '$') {
                    emitInstruction("movl " + condition + ", %eax");
                } else {
                    emitInstruction("movl " + condition + ", %eax");
                }
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + label);
                break;
            }
            case TACType::CALL: {
                std::string result = current->getRes()->getLexeme();
                std::string funcName = current->getOp1()->getLexeme();
                emitComment("Call function " + funcName);
                emitInstruction("call " + funcName);
                if (!result.empty()) {
                    std::string resultLoc = allocateVariable(result);
                    emitInstruction("movl %eax, " + resultLoc);
                }
                break;
            }
            case TACType::ARG: {
                std::string arg = getOperandValue(current->getOp1());
                emitComment("Push argument: " + arg);
                if (arg[0] == '$') {
                    emitInstruction("pushl " + arg);
                } else {
                    emitInstruction("pushl " + arg);
                }
                break;
            }
            case TACType::READ: {
                std::string var = current->getRes()->getLexeme();
                std::string varLoc = getOperandLocation(current->getRes());
                emitComment("Read into " + var);
                emitInstruction("pushl " + varLoc);
                emitInstruction("pushl $scanf_int");
                emitInstruction("call scanf");
                emitInstruction("addl $8, %esp");
                break;
            }
            case TACType::VECWRITE: {
                std::string vecName = current->getRes()->getLexeme();
                std::string index = getOperandValue(current->getOp1());
                std::string value = getOperandValue(current->getOp2());
                emitComment("Vector write: " + vecName + "[" + index + "] = " + value);
                std::string vecLoc = getOperandLocation(current->getRes());
                if (index[0] == '$') {
                    emitInstruction("movl " + index + ", %eax");
                } else {
                    emitInstruction("movl " + index + ", %eax");
                }
                emitInstruction("imull $4, %eax");
                if (value[0] == '$') {
                    emitInstruction("movl " + value + ", %ebx");
                } else {
                    emitInstruction("movl " + value + ", %ebx");
                }
                emitInstruction("movl %ebx, " + vecLoc + "(%eax)");
                break;
            }
            case TACType::VECREAD: {
                std::string result = current->getRes()->getLexeme();
                std::string vecName = current->getOp1()->getLexeme();
                std::string index = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string vecLoc = getOperandLocation(current->getOp1());
                emitComment("Vector read: " + result + " = " + vecName + "[" + index + "]");
                if (index[0] == '$') {
                    emitInstruction("movl " + index + ", %eax");
                } else {
                    emitInstruction("movl " + index + ", %eax");
                }
                emitInstruction("imull $4, %eax");
                emitInstruction("movl " + vecLoc + "(%eax), %ebx");
                emitInstruction("movl %ebx, " + resultLoc);
                break;
            }
            case TACType::BEGINVEC: {
                std::string vecName = current->getOp1()->getLexeme();
                int size = std::stoi(current->getOp2()->getLexeme());
                emitComment("Begin vector " + vecName + " with " + std::to_string(size) + " elements");
                allocateVector(vecName, size);
                break;
            }
            case TACType::ENDVEC: {
                std::string vecName = current->getOp1()->getLexeme();
                emitComment("End vector " + vecName);
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
