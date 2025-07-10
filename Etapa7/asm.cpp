// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Assembly (ASM) source code made by Nathan Alonso Guimarães (00334437)

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
#include <cstdlib>

// Global state variables
std::ofstream* outFile = nullptr;                                    // Output file stream
std::unordered_map<std::string, std::string> varLocations;          // Variable to memory location mapping
std::unordered_map<std::string, int> vectorSizes;                   // Vector name to size mapping
std::vector<std::string> stringLiterals;                            // String literals storage
int stackOffset = 0;                                                // Current stack offset for local variables
int labelCounter = 0;                                               // Counter for generating unique labels
int stringCounter = 0;                                              // Counter for string literals
std::string currentRegister = "%rax";                               // Currently used register (usually %rax)
std::string currentFunction = "";                                   // Current function being processed
std::unordered_map<std::string, int> functionParamCount;            // Function name to parameter count mapping
int currentParamOffset = 16;                                        // Current parameter offset (starts at 16(%rbp))

// Utility functions
std::string getOperandLocation(class Symbol* symbol);
std::string getOperandValue(class Symbol* symbol);
std::string allocateVariable(const std::string& name, int size = 4);
std::string allocateParameter(const std::string& name);
std::string allocateVector(const std::string& name, int elements);
std::string getStringLabel(const std::string& str);
void emitInstruction(const std::string& instruction);
void emitLabel(const std::string& label);
void emitComment(const std::string& comment);
void processInstruction(class TAC* current);
int evaluateExpression(const std::string& expression);
void loadOperandToRegister(const std::string& operand, const std::string& reg);
void storeRegisterToOperand(const std::string& reg, const std::string& operand);


std::string getOperandLocation(Symbol* symbol) {
    if (!symbol) return "";
    
    std::string name = symbol->getLexeme();
    
    // Check if already allocated
    auto it = varLocations.find(name);
    if (it != varLocations.end()) {
        return it->second;
    }
    
    // If we're in a function other than main, and this is an unknown variable,
    // treat it as a function parameter
    if (currentFunction != "main" && currentFunction != "") {
        return allocateParameter(name);
    }
    
    // Allocate new location on the stack (for non-global variables)
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
    std::string location = "-" + std::to_string(stackOffset) + "(%rbp)";
    varLocations[name] = location;
    return location;
}

std::string allocateParameter(const std::string& name) {
    auto it = varLocations.find(name);
    if (it != varLocations.end()) {
        return it->second;
    }
    
    // For user-defined functions, parameters come in registers (%edi, %esi, %edx, %ecx)
    // We'll map them to stack locations and move them there at function start
    stackOffset += 4;
    std::string location = "-" + std::to_string(stackOffset) + "(%rbp)";
    varLocations[name] = location;
    
    return location;
}

std::string allocateVector(const std::string& name, int elements) {
    return allocateVariable(name, elements * 8);  // 8 bytes per element in 64-bit
}

// Helper function to evaluate expressions like "7/1"
int evaluateExpression(const std::string& expression) {
    size_t slashPos = expression.find('/');
    if (slashPos != std::string::npos) {
        std::string numeratorStr = expression.substr(0, slashPos);
        std::string denominatorStr = expression.substr(slashPos + 1);
        
        int numerator = std::atoi(numeratorStr.c_str());
        int denominator = std::atoi(denominatorStr.c_str());
        
        if (denominator != 0) {
            return numerator / denominator;
        }
    }
    return std::atoi(expression.c_str());
}

// Helper function to load operand to register (PIE-compatible)
void loadOperandToRegister(const std::string& operand, const std::string& reg) {
    if (operand[0] == '$') {
        // Immediate value
        emitInstruction("movl " + operand + ", " + reg);
    } else if (operand.find("(%rbp)") != std::string::npos) {
        // Stack variable
        emitInstruction("movl " + operand + ", " + reg);
    } else {
        // Global variable - use PIE-compatible load
        emitInstruction("movl " + operand + "(%rip), " + reg);
    }
}

// Helper function to store register to operand (PIE-compatible)
void storeRegisterToOperand(const std::string& reg, const std::string& operand) {
    if (operand.find("(%rbp)") != std::string::npos) {
        // Stack variable
        emitInstruction("movl " + reg + ", " + operand);
    } else {
        // Global variable - use PIE-compatible store
        emitInstruction("movl " + reg + ", " + operand + "(%rip)");
    }
}

std::string getStringLabel(const std::string& str) {
    // Find existing string or add new one
    auto it = std::find(stringLiterals.begin(), stringLiterals.end(), str);
    if (it != stringLiterals.end()) {
        size_t index = std::distance(stringLiterals.begin(), it);
        return "str_" + std::to_string(index);
    }
    
    // Add new string
    stringLiterals.push_back(str);
    return "str_" + std::to_string(stringLiterals.size() - 1);
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
    currentRegister = "%rax";
    currentFunction = "";
    functionParamCount.clear();
    currentParamOffset = 16;
    
    // Check if TAC exists
    if (!tacHead) {
        file.close();
        return;
    }
    
    emitHeader("Generated x86 Assembly Code from 2025++1 code");
    emitHeader("Compiler made by Nathan Guimaraes (334437)");
    (*outFile) << std::endl;
    
    // Process in the same order as printTAC function
    // Start from the end and work backwards
    TAC* current = tacHead;
    
    // Collect vector information, string literals, and global variables first
    std::vector<std::pair<std::string, std::string>> globalVars; // name, initial_value
    std::vector<std::pair<std::string, std::vector<std::string>>> globalVectors; // name, initialization_values
    
    std::string currentVectorName = "";
    std::vector<std::string> currentVectorValues;
    while (current) {
        if (current->getType() == TACType::BEGINVEC) {
            currentVectorName = current->getOp1()->getLexeme();
            int size = std::stoi(current->getOp2()->getLexeme());
            vectorSizes[currentVectorName] = size;
            currentVectorValues.clear();
        }
        else if (current->getType() == TACType::VECWRITE && !currentVectorName.empty()) {
            // Collect initialization values for the current vector
            std::string value = current->getOp2()->getLexeme();
            currentVectorValues.push_back(value);
        }
        else if (current->getType() == TACType::ENDVEC && !currentVectorName.empty()) {
            // Store the vector with its initialization values
            globalVectors.push_back({currentVectorName, currentVectorValues});
            currentVectorName = "";
            currentVectorValues.clear();
        }
        // Collect INIT variables for global data section
        if (current->getType() == TACType::INIT) {
            std::string varName = current->getRes()->getLexeme();
            std::string value = current->getOp1()->getLexeme();
            globalVars.push_back({varName, value});
        }
        // Check for string literals in PRINT and other operations
        if (current->getType() == TACType::PRINT && current->getOp1()) {
            std::string value = current->getOp1()->getLexeme();
            if (value.front() == '"' && value.back() == '"') {
                getStringLabel(value); // This will add it to stringLiterals if not already there
            }
        }
        current = current->getNext();
    }
    
    // Generate data section
    emitLabel(".section .data");
    emitInstruction("int_format: .string \"%d\"");
    emitInstruction("char_format: .string \"%c\"");
    emitInstruction("string_format: .string \"%s\"");
    emitInstruction("newline: .string \"\\n\"");
    emitInstruction("scanf_int: .string \"%d\"");
    emitInstruction("scanf_char: .string \" %c\"");
    
    // Global variables
    for (const auto& var : globalVars) {
        std::string varName = var.first;
        std::string value = var.second;
        
        // Handle character literals
        if (value[0] == '\'' && value.length() == 3 && value[2] == '\'') {
            value = std::to_string((int)value[1]);
        }
        // Handle arithmetic expressions
        if (value.find('/') != std::string::npos) {
            int result = evaluateExpression(value);
            value = std::to_string(result);
        }
        
        emitInstruction(varName + ": .long " + value);
        // Update varLocations to point to global variable (without $ prefix for global vars)
        varLocations[varName] = varName;
    }
    
    // Global vectors
    for (const auto& vec : globalVectors) {
        std::string vecName = vec.first;
        const std::vector<std::string>& values = vec.second;
        
        emitComment("Vector " + vecName + " initialization");
        emitLabel("\t" + vecName + ":");
        
        for (const std::string& value : values) {
            std::string processedValue = value;
            
            // Handle character literals
            if (processedValue[0] == '\'' && processedValue.length() == 3 && processedValue[2] == '\'') {
                processedValue = std::to_string((int)processedValue[1]);
            }
            // Handle arithmetic expressions and octal numbers
            if (processedValue.find('/') != std::string::npos) {
                int result = evaluateExpression(processedValue);
                processedValue = std::to_string(result);
            }
            // Handle octal numbers (like 01, 001, etc.)
            if (processedValue.length() > 1 && processedValue[0] == '0' && std::isdigit(processedValue[1])) {
                int octalValue = std::stoi(processedValue, nullptr, 8);
                processedValue = std::to_string(octalValue);
            }
            
            emitInstruction("\t.long " + processedValue);
        }
        
        // Update varLocations to point to global vector
        varLocations[vecName] = vecName;
    }
    
    // String literals
    for (size_t i = 0; i < stringLiterals.size(); i++) {
        std::string label = "str_" + std::to_string(i);
        emitInstruction(label + ": .string " + stringLiterals[i]);
    }
    (*outFile) << std::endl;
    
    // Generate text section
    emitLabel(".section .text");
    emitLabel(".globl main");
    emitLabel(".extern printf");
    emitLabel(".extern scanf");
    (*outFile) << std::endl;
    
    // Process TAC instructions, but organize them properly
    // First pass: collect function definitions and global initializations
    std::vector<TAC*> globalInits;
    std::vector<TAC*> mainInstructions;
    std::vector<TAC*> functionInstructions;
    bool inMainFunction = false;
    bool inOtherFunction = false;
    bool inVectorInitialization = false;
    
    current = tacHead;
    while (current) {
        switch (current->getType()) {
            case TACType::BEGINFUN:
                if (current->getOp1()->getLexeme() == "main") {
                    inMainFunction = true;
                    // Don't add main's BEGINFUN to functionInstructions since we handle it manually
                } else {
                    inOtherFunction = true;
                    functionInstructions.push_back(current);
                }
                break;
            case TACType::ENDFUN:
                if (current->getOp1()->getLexeme() == "main") {
                    inMainFunction = false;
                    // Don't add main's ENDFUN to functionInstructions since we handle it manually
                } else {
                    functionInstructions.push_back(current);
                    inOtherFunction = false;
                }
                break;
            case TACType::INIT:
                if (!inMainFunction && !inOtherFunction) {
                    globalInits.push_back(current);
                } else if (inMainFunction) {
                    mainInstructions.push_back(current);
                } else {
                    functionInstructions.push_back(current);
                }
                break;
            case TACType::BEGINVEC:
                inVectorInitialization = true;
                if (!inMainFunction && !inOtherFunction) {
                    globalInits.push_back(current);
                }
                break;
            case TACType::ENDVEC:
                inVectorInitialization = false;
                if (!inMainFunction && !inOtherFunction) {
                    globalInits.push_back(current);
                }
                break;
            case TACType::VECWRITE:
                // Skip vector writes that are part of global initialization
                if (!inVectorInitialization) {
                    if (inMainFunction) {
                        mainInstructions.push_back(current);
                    } else if (inOtherFunction) {
                        functionInstructions.push_back(current);
                    }
                }
                break;
            default:
                if (inMainFunction) {
                    mainInstructions.push_back(current);
                } else if (inOtherFunction) {
                    functionInstructions.push_back(current);
                }
                break;
        }
        current = current->getNext();
    }
    
    // Generate main function
    emitLabel("main:");
    emitComment("Function main prologue");
    emitInstruction("pushq %rbp");
    emitInstruction("movq %rsp, %rbp");
    
    // Allocate space for temporary variables only (vectors are now global)
    // Skip INIT variables and vectors since they're now global
    for (TAC* tac : globalInits) {
        // Only process non-INIT, non-vector instructions that might need temp variables
        if (tac->getType() != TACType::INIT && tac->getType() != TACType::BEGINVEC && tac->getType() != TACType::ENDVEC) {
            // This will allocate temp variables as needed during processing
        }
    }
    for (TAC* tac : mainInstructions) {
        // Only process instructions that might need temp variables
        if (tac->getType() != TACType::INIT && tac->getType() != TACType::BEGINVEC && tac->getType() != TACType::ENDVEC) {
            // This will allocate temp variables as needed during processing
        }
    }
    
    // Subtract stack space for local variables (vectors and temporary variables)
    if (stackOffset > 0) {
        emitInstruction("subq $" + std::to_string(stackOffset) + ", %rsp");
    }
    
    // Process global initializations (now inside main)
    for (TAC* tac : globalInits) {
        processInstruction(tac);
    }
    
    // Process main function instructions
    for (TAC* tac : mainInstructions) {
        processInstruction(tac);
    }
    
    // Main function epilogue
    emitComment("Function main epilogue");
    emitInstruction("movq %rbp, %rsp");
    emitInstruction("popq %rbp");
    emitInstruction("ret");
    (*outFile) << std::endl;
    
    // Process other functions
    for (TAC* tac : functionInstructions) {
        processInstruction(tac);
    }
    
    // Add GNU stack note section to prevent execstack warning
    (*outFile) << std::endl;
    emitLabel(".section .note.GNU-stack,\"\",@progbits");
    
    file.close();
}

void processInstruction(TAC* current) {
    if (!current) return;
    
    switch (current->getType()) {
            case TACType::INIT: {
                // INIT instructions are now handled in the data section
                // Skip processing here
                break;
            }
            case TACType::MOVE: {
                std::string dest = current->getRes()->getLexeme();
                std::string src = getOperandValue(current->getOp1());
                std::string destLoc = getOperandLocation(current->getRes());
                emitComment("Move " + src + " to " + dest);
                if (src[0] == '$') {
                    // Check if destination is a global variable
                    if (destLoc.find("(%rbp)") == std::string::npos) {
                        // Global variable - use PIE-compatible store
                        emitInstruction("movl " + src + ", " + destLoc + "(%rip)");
                    } else {
                        // Stack variable
                        emitInstruction("movl " + src + ", " + destLoc);
                    }
                } else {
                    // Check if source is a global variable
                    if (src.find("(%rbp)") == std::string::npos) {
                        // Global variable - use PIE-compatible load
                        emitInstruction("movl " + src + "(%rip), %eax");
                    } else {
                        // Stack variable
                        emitInstruction("movl " + src + ", %eax");
                    }
                    // Check if destination is a global variable
                    if (destLoc.find("(%rbp)") == std::string::npos) {
                        // Global variable - use PIE-compatible store
                        emitInstruction("movl %eax, " + destLoc + "(%rip)");
                    } else {
                        // Stack variable
                        emitInstruction("movl %eax, " + destLoc);
                    }
                }
                break;
            }
            case TACType::PRINT: {
                std::string value = getOperandValue(current->getOp1());
                emitComment("Print " + value);
                std::string format = "int_format";
                
                // Integer or string printing
                if (value.front() == '"' && value.back() == '"') {
                    format = "string_format";
                    value = getStringLabel(value);
                }
                // In 64-bit System V ABI, first argument goes in %rdi, second in %rsi
                if (value[0] == '$') {
                    // Remove $ prefix and use PIE-compatible addressing
                    std::string cleanValue = value.substr(1);
                    emitInstruction("leaq " + cleanValue + "(%rip), %rsi");
                } else {
                    // Check if it's a global variable (no %rbp reference)
                    if (value.find("(%rbp)") == std::string::npos && value[0] != '$') {
                        // Check if it's a string literal or an integer variable
                        if (value.find("str_") == 0) {
                            // String literal - use address
                            emitInstruction("leaq " + value + "(%rip), %rsi");
                        } else {
                            // Global variable value - use PIE-compatible load
                            emitInstruction("movl " + value + "(%rip), %esi");
                        }
                    } else {
                        // Stack variable or immediate value
                        emitInstruction("movl " + value + ", %esi");
                    }
                }
                emitInstruction("leaq " + format + "(%rip), %rdi");
                emitInstruction("call printf@PLT");
                break;
            }
            case TACType::BEGINFUN: {
                std::string funcName = current->getOp1()->getLexeme();
                currentFunction = funcName;
                currentParamOffset = 16; // Reset parameter offset (16 in 64-bit due to return address + %rbp)
                emitLabel(funcName + ":");
                emitComment("Function " + funcName + " prologue");
                emitInstruction("pushq %rbp");
                emitInstruction("movq %rsp, %rbp");
                stackOffset = 0; // Reset local variable offset
                
                // For user-defined functions (not main), move register arguments to stack
                if (funcName != "main") {
                    // Look ahead to see what parameters this function has
                    TAC* nextTAC = current->getNext();
                    int paramCount = 0;
                    while (nextTAC && nextTAC->getType() != TACType::ENDFUN) {
                        // Count operations that use parameters (typically first operations in function)
                        if (nextTAC->getType() == TACType::ADD || nextTAC->getType() == TACType::SUB ||
                            nextTAC->getType() == TACType::MUL || nextTAC->getType() == TACType::DIV ||
                            nextTAC->getType() == TACType::MOD) {
                            // If using parameter symbols, allocate and move from registers
                            if (nextTAC->getOp1() && paramCount < 2) {
                                std::string param1Name = nextTAC->getOp1()->getLexeme();
                                if (varLocations.find(param1Name) == varLocations.end()) {
                                    std::string paramLoc = allocateParameter(param1Name);
                                    if (paramCount == 0) {
                                        emitInstruction("movl %edi, " + paramLoc);
                                    } else if (paramCount == 1) {
                                        emitInstruction("movl %esi, " + paramLoc);
                                    }
                                    paramCount++;
                                }
                            }
                            if (nextTAC->getOp2() && paramCount < 2) {
                                std::string param2Name = nextTAC->getOp2()->getLexeme();
                                if (varLocations.find(param2Name) == varLocations.end()) {
                                    std::string paramLoc = allocateParameter(param2Name);
                                    if (paramCount == 0) {
                                        emitInstruction("movl %edi, " + paramLoc);
                                    } else if (paramCount == 1) {
                                        emitInstruction("movl %esi, " + paramLoc);
                                    }
                                    paramCount++;
                                }
                            }
                            break; // Only process first operation to get parameters
                        }
                        nextTAC = nextTAC->getNext();
                    }
                }
                break;
            }
            case TACType::ENDFUN: {
                std::string funcName = current->getOp1()->getLexeme();
                emitComment("Function " + funcName + " epilogue");
                emitInstruction("movq %rbp, %rsp");
                emitInstruction("popq %rbp");
                emitInstruction("ret");
                (*outFile) << std::endl;
                break;
            }
            case TACType::RET: {
                if (current->getOp1()) {
                    std::string retVal = getOperandValue(current->getOp1());
                    emitComment("Return " + retVal);
                    loadOperandToRegister(retVal, "%eax");
                }
                break;
            }
            case TACType::ADD: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                
                // Integer addition
                emitComment("Add: " + result + " = " + op1 + " + " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("addl %ebx, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                break;
            }
            case TACType::SUB: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                
                // Integer subtraction
                emitComment("Subtract: " + result + " = " + op1 + " - " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("subl %ebx, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                break;
            }
            case TACType::MUL: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Multiply: " + result + " = " + op1 + " * " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("imull %ebx, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                break;
            }
            case TACType::DIV: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Divide: " + result + " = " + op1 + " / " + op2);
                loadOperandToRegister(op1, "%eax");
                emitInstruction("cdq"); // Sign extend %eax to %edx:%eax
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("idivl %ebx");
                storeRegisterToOperand("%eax", resultLoc);
                break;
            }
            case TACType::MOD: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                emitComment("Modulo: " + result + " = " + op1 + " % " + op2);
                loadOperandToRegister(op1, "%eax");
                emitInstruction("cdq"); // Sign extend %eax to %edx:%eax
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("idivl %ebx");
                storeRegisterToOperand("%edx", resultLoc); // Remainder is in %edx
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
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("jl " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
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
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("je " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::NE: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "ne_true_" + std::to_string(labelCounter++);
                std::string endLabel = "ne_end_" + std::to_string(labelCounter++);
                emitComment("Not Equal: " + result + " = " + op1 + " != " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("jne " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::GT: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "gt_true_" + std::to_string(labelCounter++);
                std::string endLabel = "gt_end_" + std::to_string(labelCounter++);
                emitComment("Greater than: " + result + " = " + op1 + " > " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("jg " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::LE: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "le_true_" + std::to_string(labelCounter++);
                std::string endLabel = "le_end_" + std::to_string(labelCounter++);
                emitComment("Less or Equal: " + result + " = " + op1 + " <= " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("jle " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::GE: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "ge_true_" + std::to_string(labelCounter++);
                std::string endLabel = "ge_end_" + std::to_string(labelCounter++);
                emitComment("Greater or Equal: " + result + " = " + op1 + " >= " + op2);
                loadOperandToRegister(op1, "%eax");
                loadOperandToRegister(op2, "%ebx");
                emitInstruction("cmpl %ebx, %eax");
                emitInstruction("jge " + trueLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
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
                loadOperandToRegister(op1, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + zeroLabel);
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                emitLabel(zeroLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::AND: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string falseLabel = "and_false_" + std::to_string(labelCounter++);
                std::string endLabel = "and_end_" + std::to_string(labelCounter++);
                emitComment("Logical AND: " + result + " = " + op1 + " && " + op2);
                
                // Short-circuit evaluation: if op1 is false, result is false
                loadOperandToRegister(op1, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + falseLabel);
                
                // Check op2: if op2 is false, result is false
                loadOperandToRegister(op2, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + falseLabel);
                
                // Both are true, result is true
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                
                emitLabel(falseLabel + ":");
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitLabel(endLabel + ":");
                break;
            }
            case TACType::OR: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string trueLabel = "or_true_" + std::to_string(labelCounter++);
                std::string endLabel = "or_end_" + std::to_string(labelCounter++);
                emitComment("Logical OR: " + result + " = " + op1 + " || " + op2);
                
                // Short-circuit evaluation: if op1 is true, result is true
                loadOperandToRegister(op1, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("jne " + trueLabel);
                
                // Check op2: if op2 is true, result is true
                loadOperandToRegister(op2, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("jne " + trueLabel);
                
                // Both are false, result is false
                emitInstruction("movl $0, %eax");
                storeRegisterToOperand("%eax", resultLoc);
                emitInstruction("jmp " + endLabel);
                
                emitLabel(trueLabel + ":");
                emitInstruction("movl $1, %eax");
                storeRegisterToOperand("%eax", resultLoc);
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
                loadOperandToRegister(condition, "%eax");
                emitInstruction("cmpl $0, %eax");
                emitInstruction("je " + label);
                break;
            }
            case TACType::CALL: {
                std::string result = current->getRes()->getLexeme();
                std::string funcName = current->getOp1()->getLexeme();
                emitComment("Call function " + funcName);
                
                // Collect the ARG instructions before this CALL
                std::vector<std::string> args;
                TAC* temp = current->getPrev();
                while (temp && temp->getType() == TACType::ARG) {
                    std::string arg = getOperandValue(temp->getOp1());
                    args.push_back(arg);
                    temp = temp->getPrev();
                }
                
                // Reverse args to get correct order (first arg in args[0])
                std::reverse(args.begin(), args.end());
                
                // Skip string arguments for user-defined functions (these are for print formatting)
                std::vector<std::string> functionArgs;
                for (const std::string& arg : args) {
                    if (!(arg.front() == '"' && arg.back() == '"')) {
                        functionArgs.push_back(arg);
                    }
                }
                
                // Pass arguments via registers for user-defined functions
                if (funcName != "printf" && funcName != "scanf") {
                    if (functionArgs.size() >= 1) {
                        loadOperandToRegister(functionArgs[0], "%edi");
                    }
                    if (functionArgs.size() >= 2) {
                        loadOperandToRegister(functionArgs[1], "%esi");
                    }
                    // For more than 2 args, we'd need %edx, %ecx, etc.
                }
                
                // For external functions, use PLT
                if (funcName == "printf" || funcName == "scanf") {
                    emitInstruction("call " + funcName + "@PLT");
                } else {
                    // For internal functions, direct call
                    emitInstruction("call " + funcName);
                }
                
                if (!result.empty()) {
                    std::string resultLoc = allocateVariable(result);
                    storeRegisterToOperand("%eax", resultLoc);
                }
                break;
            }
            case TACType::ARG: {
                // In 64-bit System V ABI, arguments are passed in registers
                // (%rdi, %rsi, %rdx, %rcx, %r8, %r9), then on stack
                // For simplicity, collect arguments and pass them when CALL happens
                break;
            }
            case TACType::READ: {
                std::string var = current->getRes()->getLexeme();
                std::string varLoc = getOperandLocation(current->getRes());
                emitComment("Read into " + var);
                // In 64-bit System V ABI, first argument goes in %rdi, second in %rsi
                // For global variables, we need the address (using PIE-compatible addressing)
                // For stack variables, we need to use lea to get address
                if (varLoc.find("(%rbp)") == std::string::npos) {
                    // Global variable - use PIE-compatible addressing
                    emitInstruction("leaq " + varLoc + "(%rip), %rsi");
                } else {
                    // Stack variable - use lea to get effective address
                    emitInstruction("leaq " + varLoc + ", %rsi");
                }
                emitInstruction("leaq scanf_int(%rip), %rdi");
                emitInstruction("call scanf@PLT");
                break;
            }
            case TACType::VECWRITE: {
                std::string vecName = current->getRes()->getLexeme();
                std::string index = getOperandValue(current->getOp1());
                std::string value = getOperandValue(current->getOp2());
                emitComment("Vector write: " + vecName + "[" + index + "] = " + value);
                std::string vecLoc = getOperandLocation(current->getRes());
                
                // Load index using PIE-compatible method
                loadOperandToRegister(index, "%eax");
                emitInstruction("imull $4, %eax"); // 4 bytes per element for int values
                // Load value using PIE-compatible method
                loadOperandToRegister(value, "%ebx");
                
                // For global vectors, use PIE-compatible addressing
                if (vecLoc.find("(%rbp)") == std::string::npos) {
                    // Global vector - use PIE-compatible addressing with index
                    emitInstruction("leaq " + vecLoc + "(%rip), %rcx");
                    emitInstruction("movl %ebx, (%rcx,%rax)");
                } else {
                    // Stack vector (shouldn't happen anymore, but keep for compatibility)
                    size_t parenPos = vecLoc.find('(');
                    std::string offset = vecLoc.substr(0, parenPos);
                    std::string reg = vecLoc.substr(parenPos + 1, vecLoc.length() - parenPos - 2);
                    emitInstruction("movl %ebx, " + offset + "(" + reg + ",%rax)");
                }
                break;
            }
            case TACType::VECREAD: {
                std::string result = current->getRes()->getLexeme();
                std::string vecName = current->getOp1()->getLexeme();
                std::string index = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                std::string vecLoc = getOperandLocation(current->getOp1());
                emitComment("Vector read: " + result + " = " + vecName + "[" + index + "]");
                
                // Load index using PIE-compatible method
                loadOperandToRegister(index, "%eax");
                emitInstruction("imull $4, %eax"); // 4 bytes per element for int values
                
                // For global vectors, use PIE-compatible addressing
                if (vecLoc.find("(%rbp)") == std::string::npos) {
                    // Global vector - use PIE-compatible addressing with index
                    emitInstruction("leaq " + vecLoc + "(%rip), %rcx");
                    emitInstruction("movl (%rcx,%rax), %ebx");
                } else {
                    // Stack vector (shouldn't happen anymore, but keep for compatibility)
                    size_t parenPos = vecLoc.find('(');
                    std::string offset = vecLoc.substr(0, parenPos);
                    std::string reg = vecLoc.substr(parenPos + 1, vecLoc.length() - parenPos - 2);
                    emitInstruction("movl " + offset + "(" + reg + ",%eax), %ebx");
                }
                emitInstruction("movl %ebx, " + resultLoc);
                break;
            }
            case TACType::BEGINVEC: {
                // BEGINVEC instructions are now handled in the data section
                // Skip processing here
                break;
            }
            case TACType::ENDVEC: {
                // ENDVEC instructions are now handled in the data section
                // Skip processing here
                break;
            }
            default:
                emitComment("Unhandled TAC instruction");
                break;
    }
}
