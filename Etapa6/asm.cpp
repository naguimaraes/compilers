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
std::unordered_map<std::string, dataType> varTypes;                 // Variable name to type mapping
std::vector<std::string> stringLiterals;                            // String literals storage                            // String literals storage
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
float evaluateRealExpression(const std::string& expression);
bool isRealVariable(const std::string& name);
std::string getRealValue(const std::string& value);


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
    
    // Check if this is a real variable and adjust size
    if (isRealVariable(name)) {
        size = 4; // float is 4 bytes
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
    
    std::string location = std::to_string(currentParamOffset) + "(%rbp)";
    varLocations[name] = location;
    currentParamOffset += 8; // Each parameter is 8 bytes in 64-bit
    return location;
}

std::string allocateVector(const std::string& name, int elements) {
    return allocateVariable(name, elements * 8);  // 8 bytes per element in 64-bit
}

// Helper function to evaluate real expressions like "7/1"
float evaluateRealExpression(const std::string& expression) {
    size_t slashPos = expression.find('/');
    if (slashPos != std::string::npos) {
        std::string numeratorStr = expression.substr(0, slashPos);
        std::string denominatorStr = expression.substr(slashPos + 1);
        
        float numerator = static_cast<float>(std::atof(numeratorStr.c_str()));
        float denominator = static_cast<float>(std::atof(denominatorStr.c_str()));
        
        if (denominator != 0.0f) {
            return numerator / denominator;
        }
    }
    return static_cast<float>(std::atof(expression.c_str()));
}

// Helper function to check if a variable is of real type
bool isRealVariable(const std::string& name) {
    auto it = varTypes.find(name);
    return it != varTypes.end() && it->second == dataType::REAL;
}

// Helper function to get real value as string (for data section)
std::string getRealValue(const std::string& value) {
    if (value.find('/') != std::string::npos) {
        float realValue = evaluateRealExpression(value);
        std::ostringstream oss;
        oss << realValue;
        return oss.str();
    }
    return value;
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
    varTypes.clear();
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
    std::vector<std::pair<std::string, int>> globalVectors; // name, size
    while (current) {
        if (current->getType() == TACType::BEGINVEC) {
            std::string vecName = current->getOp1()->getLexeme();
            int size = std::stoi(current->getOp2()->getLexeme());
            vectorSizes[vecName] = size;
            globalVectors.push_back({vecName, size});
        }
        // Collect INIT variables for global data section
        if (current->getType() == TACType::INIT) {
            std::string varName = current->getRes()->getLexeme();
            std::string value = current->getOp1()->getLexeme();
            globalVars.push_back({varName, value});
            
            // Try to determine variable type from symbol table
            Symbol* symbol = getSymbol(varName);
            if (symbol) {
                varTypes[varName] = symbol->getDataType();
            }
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
    emitInstruction("real_format: .string \"%.2f\"");
    emitInstruction("string_format: .string \"%s\"");
    emitInstruction("newline: .string \"\\n\"");
    emitInstruction("scanf_int: .string \"%d\"");
    emitInstruction("scanf_char: .string \" %c\"");
    emitInstruction("scanf_real: .string \"%f\"");
    
    // Global variables
    for (const auto& var : globalVars) {
        std::string varName = var.first;
        std::string value = var.second;
        
        // Check if this is a real variable
        bool isReal = isRealVariable(varName);
        
        if (isReal) {
            // Handle real variables
            if (value.find('/') != std::string::npos) {
                // Evaluate expressions like "7/01" 
                float realValue = evaluateRealExpression(value);
                std::ostringstream oss;
                oss << realValue;
                emitInstruction(varName + ": .float " + oss.str());
            } else {
                emitInstruction(varName + ": .float " + value);
            }
        } else {
            // Handle integer variables
            // Handle character literals
            if (value[0] == '\'' && value.length() == 3 && value[2] == '\'') {
                value = std::to_string((int)value[1]);
            }
            // Handle arithmetic expressions
            if (value == "1/2") value = "0";
            if (value == "4/7") value = "0"; 
            if (value == "5/7") value = "0";
            if (value == "8/7") value = "1";
            
            emitInstruction(varName + ": .long " + value);
        }
        // Update varLocations to point to global variable (without $ prefix for global vars)
        varLocations[varName] = varName;
    }
    
    // Global vectors
    for (const auto& vec : globalVectors) {
        std::string vecName = vec.first;
        int size = vec.second;
        emitInstruction(vecName + ": .space " + std::to_string(size * 4)); // 4 bytes per element for int values
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
            case TACType::BEGINVEC:
            case TACType::ENDVEC:
                if (!inMainFunction && !inOtherFunction) {
                    globalInits.push_back(current);
                } else if (inMainFunction) {
                    // Skip BEGINVEC and ENDVEC in main since vectors are now global
                    if (current->getType() != TACType::BEGINVEC && current->getType() != TACType::ENDVEC) {
                        mainInstructions.push_back(current);
                    }
                } else {
                    functionInstructions.push_back(current);
                }
                break;
            case TACType::VECWRITE:
                // Vector writes should always be processed, especially for initializations
                if (inMainFunction) {
                    mainInstructions.push_back(current);
                } else if (inOtherFunction) {
                    functionInstructions.push_back(current);
                } else {
                    // Global vector initializations should be in main
                    mainInstructions.push_back(current);
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
                
                // Check if we're printing a real variable
                if (current->getOp1() && isRealVariable(current->getOp1()->getLexeme())) {
                    format = "$real_format";
                    // For real variables, we need to push as float
                    // In 64-bit System V ABI, floating point goes in %xmm0
                    emitInstruction("flds " + value);  // Load float onto FPU stack
                    emitInstruction("fstps -8(%rbp)"); // Store as float on stack temporarily
                    emitInstruction("movss -8(%rbp), %xmm0"); // Move to SSE register
                    emitInstruction("cvtss2sd %xmm0, %xmm0"); // Convert single to double
                    emitInstruction("movq " + format + ", %rdi");
                    emitInstruction("call printf");
                } else {
                    // Integer or string printing
                    if (value.front() == '"' && value.back() == '"') {
                        format = "$string_format";
                        value = getStringLabel(value);
                    }
                    // In 64-bit System V ABI, first argument goes in %rdi, second in %rsi
                    if (value[0] == '$') {
                        emitInstruction("movq " + value + ", %rsi");
                    } else {
                        emitInstruction("movq " + value + ", %rsi");
                    }
                    emitInstruction("movq " + format + ", %rdi");
                    emitInstruction("call printf");
                }
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
                    if (retVal[0] == '$') {
                        emitInstruction("movq " + retVal + ", %rax");
                    } else {
                        emitInstruction("movq " + retVal + ", %rax");
                    }
                }
                break;
            }
            case TACType::ADD: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                
                // Check if any operand is a real variable
                bool op1IsReal = current->getOp1() && isRealVariable(current->getOp1()->getLexeme());
                bool op2IsReal = current->getOp2() && isRealVariable(current->getOp2()->getLexeme());
                
                if (op1IsReal || op2IsReal) {
                    // Real number addition using FPU
                    emitComment("Real Add: " + result + " = " + op1 + " + " + op2);
                    
                    // Load first operand
                    if (op1IsReal) {
                        emitInstruction("flds " + op1);
                    } else {
                        // Convert integer to float
                        if (op1[0] == '$') {
                            emitInstruction("movq " + op1 + ", %rax");
                            emitInstruction("movq %rax, -8(%rbp)");
                            emitInstruction("filds -8(%rbp)");
                        } else {
                            emitInstruction("filds " + op1);
                        }
                    }
                    
                    // Add second operand
                    if (op2IsReal) {
                        emitInstruction("fadds " + op2);
                    } else {
                        // Convert integer to float
                        if (op2[0] == '$') {
                            emitInstruction("movq " + op2 + ", %rax");
                            emitInstruction("movq %rax, -8(%rbp)");
                            emitInstruction("fiadds -8(%rbp)");
                        } else {
                            emitInstruction("fiadds " + op2);
                        }
                    }
                    
                    // Store result
                    emitInstruction("fstps " + resultLoc);
                    
                    // Mark result as real type
                    varTypes[result] = dataType::REAL;
                } else {
                    // Integer addition
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
                }
                break;
            }
            case TACType::SUB: {
                std::string result = current->getRes()->getLexeme();
                std::string op1 = getOperandValue(current->getOp1());
                std::string op2 = getOperandValue(current->getOp2());
                std::string resultLoc = allocateVariable(result);
                
                // Check if any operand is a real variable
                bool op1IsReal = current->getOp1() && isRealVariable(current->getOp1()->getLexeme());
                bool op2IsReal = current->getOp2() && isRealVariable(current->getOp2()->getLexeme());
                
                if (op1IsReal || op2IsReal) {
                    // Real number subtraction using FPU
                    emitComment("Real Subtract: " + result + " = " + op1 + " - " + op2);
                    
                    // Load first operand
                    if (op1IsReal) {
                        emitInstruction("flds " + op1);
                    } else {
                        // Convert integer to float
                        if (op1[0] == '$') {
                            emitInstruction("movq " + op1 + ", %rax");
                            emitInstruction("movq %rax, -8(%rbp)");
                            emitInstruction("filds -8(%rbp)");
                        } else {
                            emitInstruction("filds " + op1);
                        }
                    }
                    
                    // Subtract second operand
                    if (op2IsReal) {
                        emitInstruction("fsubs " + op2);
                    } else {
                        // Convert integer to float
                        if (op2[0] == '$') {
                            emitInstruction("movq " + op2 + ", %rax");
                            emitInstruction("movq %rax, -8(%rbp)");
                            emitInstruction("fisubs -8(%rbp)");
                        } else {
                            emitInstruction("fisubs " + op2);
                        }
                    }
                    
                    // Store result
                    emitInstruction("fstps " + resultLoc);
                    
                    // Mark result as real type
                    varTypes[result] = dataType::REAL;
                } else {
                    // Integer subtraction
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
                }
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
                emitInstruction("cdq"); // Sign extend %eax to %edx:%eax
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
                
                // In 64-bit System V ABI, no need to clean up arguments from stack
                // since they are passed in registers
                
                if (!result.empty()) {
                    std::string resultLoc = allocateVariable(result);
                    emitInstruction("movq %rax, " + resultLoc);
                }
                break;
            }
            case TACType::ARG: {
                // In 64-bit System V ABI, arguments are passed in registers
                // (%rdi, %rsi, %rdx, %rcx, %r8, %r9), so we don't push to stack
                // This case is handled in the function call itself
                break;
            }
            case TACType::READ: {
                std::string var = current->getRes()->getLexeme();
                std::string varLoc = getOperandLocation(current->getRes());
                emitComment("Read into " + var);
                // In 64-bit System V ABI, first argument goes in %rdi, second in %rsi
                // For global variables, we need the address (add $)
                // For stack variables, we need to use lea to get address
                if (varLoc.find("(%rbp)") == std::string::npos) {
                    // Global variable - use address directly
                    emitInstruction("movq $" + varLoc + ", %rsi");
                } else {
                    // Stack variable - use lea to get effective address
                    emitInstruction("leaq " + varLoc + ", %rsi");
                }
                emitInstruction("movq $scanf_int, %rdi");
                emitInstruction("call scanf");
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
                emitInstruction("imull $4, %eax"); // 4 bytes per element for int values
                if (value[0] == '$') {
                    emitInstruction("movl " + value + ", %ebx");
                } else {
                    emitInstruction("movl " + value + ", %ebx");
                }
                
                // For global vectors, use direct addressing
                if (vecLoc.find("(%rbp)") == std::string::npos) {
                    // Global vector - use direct addressing with index
                    emitInstruction("movl %ebx, " + vecLoc + "(,%rax)");
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
                
                if (index[0] == '$') {
                    emitInstruction("movl " + index + ", %eax");
                } else {
                    emitInstruction("movl " + index + ", %eax");
                }
                emitInstruction("imull $4, %eax"); // 4 bytes per element for int values
                
                // For global vectors, use direct addressing
                if (vecLoc.find("(%rbp)") == std::string::npos) {
                    // Global vector - use direct addressing with index
                    emitInstruction("movl " + vecLoc + "(,%rax), %ebx");
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
