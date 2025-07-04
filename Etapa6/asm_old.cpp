#include "asm.hpp"
#include "tac.hpp"
#include "symbol.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <vector>

// Global state variables - formerly class members
static std::ofstream* g_outFile = nullptr;                                    // Output file stream
static std::unordered_map<std::string, std::string> g_varLocations;          // Variable to memory location mapping
static std::unordered_map<std::string, int> g_vectorSizes;                   // Vector name to size mapping
static std::vector<std::string> g_stringLiterals;                            // String literals storage
static int g_stackOffset = 0;                                                // Current stack offset for local variables
static int g_labelCounter = 0;                                               // Counter for generating unique labels
static int g_stringCounter = 0;                                              // Counter for string literals
static std::string g_currentRegister = "%eax";                               // Currently used register (usually %eax)

// Forward declarations of internal functions
static void generateDataSection();
static void generateTextSection();
static void generatePrologue();
static void generateEpilogue();

// TAC instruction handlers
static void handleInit(TAC* tac);
static void handleMove(TAC* tac);
static void handleAdd(TAC* tac);
static void handleSub(TAC* tac);
static void handleMul(TAC* tac);
static void handleDiv(TAC* tac);
static void handleMod(TAC* tac);
static void handleAnd(TAC* tac);
static void handleOr(TAC* tac);
static void handleNot(TAC* tac);
static void handleLt(TAC* tac);
static void handleGt(TAC* tac);
static void handleLe(TAC* tac);
static void handleGe(TAC* tac);
static void handleEq(TAC* tac);
static void handleNe(TAC* tac);
static void handleLabel(TAC* tac);
static void handleJump(TAC* tac);
static void handleIfz(TAC* tac);
static void handleBeginFun(TAC* tac);
static void handleEndFun(TAC* tac);
static void handleCall(TAC* tac);
static void handleArg(TAC* tac);
static void handleRet(TAC* tac);
static void handlePrint(TAC* tac);
static void handleRead(TAC* tac);
static void handleVecWrite(TAC* tac);
static void handleVecRead(TAC* tac);
static void handleBeginVec(TAC* tac);
static void handleEndVec(TAC* tac);

// Utility functions
static std::string getOperandLocation(class Symbol* symbol);
static std::string getOperandValue(class Symbol* symbol);
static std::string allocateVariable(const std::string& name, int size = 4);
static std::string allocateVector(const std::string& name, int elements);
static std::string getStringLabel(const std::string& str);
static void saveRegisters();
static void restoreRegisters();
static void emitInstruction(const std::string& instruction);
static void emitLabel(const std::string& label);
static void emitComment(const std::string& comment);

// Core assembly generation function
static void generateAssembly(TAC* tacHead);

static void generateAssembly(TAC* tacHead) {
    if (!tacHead) return;
    
    emitComment("Generated x86 Assembly Code from TAC");
    emitComment("Compiled with 2025++1 Compiler");
    (*g_outFile) << std::endl;
    
    // Process in the same order as printTAC function
    // Start from the end and work backwards
    TAC* current = tacHead;
    
    // Collect vector information first
    while (current) {
        if (current->getType() == TACType::BEGINVEC) {
            std::string vecName = current->getOp1()->getLexeme();
            int size = std::stoi(current->getOp2()->getLexeme());
            g_vectorSizes[vecName] = size;
        }
        current = current->getPrev();
    }
    
    generateDataSection();
    generateTextSection();
    
    // Process TAC instructions in the same order as printTAC
    current = tacHead;
    while (current) {
        switch (current->getType()) {
            case TACType::INIT:
                handleInit(current);
                break;
            case TACType::MOVE:
                handleMove(current);
                break;
            case TACType::ADD:
                handleAdd(current);
                break;
            case TACType::SUB:
                handleSub(current);
                break;
            case TACType::MUL:
                handleMul(current);
                break;
            case TACType::DIV:
                handleDiv(current);
                break;
            case TACType::MOD:
                handleMod(current);
                break;
            case TACType::AND:
                handleAnd(current);
                break;
            case TACType::OR:
                handleOr(current);
                break;
            case TACType::NOT:
                handleNot(current);
                break;
            case TACType::LT:
                handleLt(current);
                break;
            case TACType::GT:
                handleGt(current);
                break;
            case TACType::LE:
                handleLe(current);
                break;
            case TACType::GE:
                handleGe(current);
                break;
            case TACType::EQ:
                handleEq(current);
                break;
            case TACType::NE:
                handleNe(current);
                break;
            case TACType::LABEL:
                handleLabel(current);
                break;
            case TACType::JUMP:
                handleJump(current);
                break;
            case TACType::IFZ:
                handleIfz(current);
                break;
            case TACType::BEGINFUN:
                handleBeginFun(current);
                break;
            case TACType::ENDFUN:
                handleEndFun(current);
                break;
            case TACType::CALL:
                handleCall(current);
                break;
            case TACType::ARG:
                handleArg(current);
                break;
            case TACType::RET:
                handleRet(current);
                break;
            case TACType::PRINT:
                handlePrint(current);
                break;
            case TACType::READ:
                handleRead(current);
                break;
            case TACType::VECWRITE:
                handleVecWrite(current);
                break;
            case TACType::VECREAD:
                handleVecRead(current);
                break;
            case TACType::BEGINVEC:
                handleBeginVec(current);
                break;
            case TACType::ENDVEC:
                handleEndVec(current);
                break;
            default:
                emitComment("Unhandled TAC instruction");
                break;
        }
        current = current->getPrev();
    }
}

static void generateDataSection() {
    emitLabel(".section .data");
    
    // Printf format strings
    emitInstruction("int_format: .string \"%d\"");
    emitInstruction("char_format: .string \"%c\"");
    emitInstruction("real_format: .string \"%.2f\"");
    emitInstruction("string_format: .string \"%s\"");
    emitInstruction("newline: .string \"\\n\"");
    emitInstruction("scanf_int: .string \"%d\"");
    emitInstruction("scanf_char: .string \" %c\"");
    emitInstruction("scanf_real: .string \"%f\"");
    
    // String literals
    for (size_t i = 0; i < g_stringLiterals.size(); i++) {
        std::string label = "str_" + std::to_string(i);
        emitInstruction(label + ": .string " + g_stringLiterals[i]);
    }
    
    (*g_outFile) << std::endl;
}

static void generateTextSection() {
    emitLabel(".section .text");
    emitLabel(".globl _start");
    
    // Main entry point
    emitLabel("_start:");
    emitInstruction("call main");
    emitInstruction("movl %eax, %ebx");  // Exit code
    emitInstruction("movl $1, %eax");    // sys_exit
    emitInstruction("int $0x80");        // System call
    
    // External function declarations
    emitLabel(".extern printf");
    emitLabel(".extern scanf");
    
    (*g_outFile) << std::endl;
}

static void handleInit(TAC* tac) {
    std::string varName = tac->getRes()->getLexeme();
    std::string value = getOperandValue(tac->getOp1());
    
    std::string location = allocateVariable(varName);
    
    emitComment("Initialize " + varName + " = " + value);
    emitInstruction("movl " + value + ", " + location);
}

static void handleMove(TAC* tac) {
    std::string dest = tac->getRes()->getLexeme();
    std::string src = getOperandValue(tac->getOp1());
    
    std::string destLoc = getOperandLocation(tac->getRes());
    
    emitComment("Move " + src + " to " + dest);
    
    if (src[0] == '$') {
        // Immediate value
        emitInstruction("movl " + src + ", " + destLoc);
    } else {
        // Variable or register
        emitInstruction("movl " + src + ", %eax");
        emitInstruction("movl %eax, " + destLoc);
    }
}

static void handleAdd(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Add: " + result + " = " + op1 + " + " + op2);
    
    // Load first operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Add second operand
    if (op2[0] == '$') {
        emitInstruction("addl " + op2 + ", %eax");
    } else {
        emitInstruction("addl " + op2 + ", %eax");
    }
    
    // Store result
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleSub(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Subtract: " + result + " = " + op1 + " - " + op2);
    
    // Load first operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Subtract second operand
    if (op2[0] == '$') {
        emitInstruction("subl " + op2 + ", %eax");
    } else {
        emitInstruction("subl " + op2 + ", %eax");
    }
    
    // Store result
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleMul(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Multiply: " + result + " = " + op1 + " * " + op2);
    
    // Load first operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Load second operand to %ebx
    if (op2[0] == '$') {
        emitInstruction("movl " + op2 + ", %ebx");
    } else {
        emitInstruction("movl " + op2 + ", %ebx");
    }
    
    // Multiply
    emitInstruction("imull %ebx, %eax");
    
    // Store result
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleDiv(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Divide: " + result + " = " + op1 + " / " + op2);
    
    // Load dividend
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Clear %edx for division
    emitInstruction("cdq");
    
    // Load divisor to %ebx
    if (op2[0] == '$') {
        emitInstruction("movl " + op2 + ", %ebx");
    } else {
        emitInstruction("movl " + op2 + ", %ebx");
    }
    
    // Divide
    emitInstruction("idivl %ebx");
    
    // Store result (quotient in %eax)
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleMod(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Modulo: " + result + " = " + op1 + " % " + op2);
    
    // Load dividend
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Clear %edx for division
    emitInstruction("cdq");
    
    // Load divisor to %ebx
    if (op2[0] == '$') {
        emitInstruction("movl " + op2 + ", %ebx");
    } else {
        emitInstruction("movl " + op2 + ", %ebx");
    }
    
    // Divide
    emitInstruction("idivl %ebx");
    
    // Store result (remainder in %edx)
    emitInstruction("movl %edx, " + resultLoc);
}

static void handleLt(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "lt_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "lt_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Less than: " + result + " = " + op1 + " < " + op2);
    
    // Load and compare operands
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
    
    // Jump if less than
    emitInstruction("jl " + trueLabel);
    
    // False case
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    
    // True case
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    
    emitLabel(endLabel + ":");
}

static void handleEq(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "eq_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "eq_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Equal: " + result + " = " + op1 + " == " + op2);
    
    // Load and compare operands
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
    
    // Jump if equal
    emitInstruction("je " + trueLabel);
    
    // False case
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    
    // True case
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    
    emitLabel(endLabel + ":");
}

static void handleLabel(TAC* tac) {
    std::string label = tac->getOp1()->getLexeme();
    emitLabel(label + ":");
}

static void handleJump(TAC* tac) {
    std::string label = tac->getOp1()->getLexeme();
    emitInstruction("jmp " + label);
}

static void handleIfz(TAC* tac) {
    std::string condition = getOperandValue(tac->getOp1());
    std::string label = tac->getOp2()->getLexeme();
    
    emitComment("Jump if zero to " + label);
    
    if (condition[0] == '$') {
        emitInstruction("movl " + condition + ", %eax");
    } else {
        emitInstruction("movl " + condition + ", %eax");
    }
    
    emitInstruction("cmpl $0, %eax");
    emitInstruction("je " + label);
}

static void handleBeginFun(TAC* tac) {
    std::string funcName = tac->getOp1()->getLexeme();
    
    emitLabel(funcName + ":");
    emitComment("Function " + funcName + " prologue");
    emitInstruction("pushl %ebp");
    emitInstruction("movl %esp, %ebp");
    
    // Reset stack offset for local variables
    g_stackOffset = 0;
}

static void handleEndFun(TAC* tac) {
    std::string funcName = tac->getOp1()->getLexeme();
    
    emitComment("Function " + funcName + " epilogue");
    emitInstruction("movl %ebp, %esp");
    emitInstruction("popl %ebp");
    emitInstruction("ret");
    (*g_outFile) << std::endl;
}

static void handleCall(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string funcName = tac->getOp1()->getLexeme();
    
    emitComment("Call function " + funcName);
    emitInstruction("call " + funcName);
    
    // Store return value
    if (!result.empty()) {
        std::string resultLoc = allocateVariable(result);
        emitInstruction("movl %eax, " + resultLoc);
    }
}

static void handleArg(TAC* tac) {
    std::string arg = getOperandValue(tac->getOp1());
    
    emitComment("Push argument: " + arg);
    
    if (arg[0] == '$') {
        emitInstruction("pushl " + arg);
    } else {
        emitInstruction("pushl " + arg);
    }
}

static void handleRet(TAC* tac) {
    if (tac->getOp1()) {
        std::string retVal = getOperandValue(tac->getOp1());
        
        emitComment("Return " + retVal);
        
        if (retVal[0] == '$') {
            emitInstruction("movl " + retVal + ", %eax");
        } else {
            emitInstruction("movl " + retVal + ", %eax");
        }
    }
}

static void handlePrint(TAC* tac) {
    std::string value = getOperandValue(tac->getOp1());
    
    emitComment("Print " + value);
    
    // Determine type and format
    std::string format = "$int_format";  // Default to integer
    
    // Check if it's a string literal
    if (value.front() == '"' && value.back() == '"') {
        format = "$string_format";
        value = getStringLabel(value);
    }
    
    // Push arguments for printf (right to left)
    if (value[0] == '$') {
        emitInstruction("pushl " + value);
    } else {
        emitInstruction("pushl " + value);
    }
    emitInstruction("pushl " + format);
    emitInstruction("call printf");
    emitInstruction("addl $8, %esp");  // Clean up stack (2 arguments * 4 bytes)
}

static void handleRead(TAC* tac) {
    std::string var = tac->getRes()->getLexeme();
    std::string varLoc = getOperandLocation(tac->getRes());
    
    emitComment("Read into " + var);
    
    // Push arguments for scanf
    emitInstruction("pushl " + varLoc);
    emitInstruction("pushl $scanf_int");
    emitInstruction("call scanf");
    emitInstruction("addl $8, %esp");  // Clean up stack
}

static void handleVecWrite(TAC* tac) {
    std::string vecName = tac->getRes()->getLexeme();
    std::string index = getOperandValue(tac->getOp1());
    std::string value = getOperandValue(tac->getOp2());
    
    emitComment("Vector write: " + vecName + "[" + index + "] = " + value);
    
    // Calculate address: base + index * 4
    std::string vecLoc = getOperandLocation(tac->getRes());
    
    // Load index
    if (index[0] == '$') {
        emitInstruction("movl " + index + ", %eax");
    } else {
        emitInstruction("movl " + index + ", %eax");
    }
    
    // Multiply by 4 (size of int)
    emitInstruction("imull $4, %eax");
    
    // Load value
    if (value[0] == '$') {
        emitInstruction("movl " + value + ", %ebx");
    } else {
        emitInstruction("movl " + value + ", %ebx");
    }
    
    // Store at calculated address
    emitInstruction("movl %ebx, " + vecLoc + "(%eax)");
}

static void handleVecRead(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string vecName = tac->getOp1()->getLexeme();
    std::string index = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string vecLoc = getOperandLocation(tac->getOp1());
    
    emitComment("Vector read: " + result + " = " + vecName + "[" + index + "]");
    
    // Calculate address: base + index * 4
    if (index[0] == '$') {
        emitInstruction("movl " + index + ", %eax");
    } else {
        emitInstruction("movl " + index + ", %eax");
    }
    
    // Multiply by 4 (size of int)
    emitInstruction("imull $4, %eax");
    
    // Load from calculated address
    emitInstruction("movl " + vecLoc + "(%eax), %ebx");
    
    // Store result
    emitInstruction("movl %ebx, " + resultLoc);
}

static void handleBeginVec(TAC* tac) {
    std::string vecName = tac->getOp1()->getLexeme();
    int size = std::stoi(tac->getOp2()->getLexeme());
    
    emitComment("Begin vector " + vecName + " with " + std::to_string(size) + " elements");
    allocateVector(vecName, size);
}

static void handleEndVec(TAC* tac) {
    std::string vecName = tac->getOp1()->getLexeme();
    emitComment("End vector " + vecName);
}

// Implement remaining comparison operations similarly...
static void handleGt(TAC* tac) {
    // Similar to handleLt but with jg instead of jl
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "gt_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "gt_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Greater than: " + result + " = " + op1 + " > " + op2);
    
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
    
    emitInstruction("jg " + trueLabel);
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    emitLabel(endLabel + ":");
}

static void handleLe(TAC* tac) {
    // Less or equal - use jle
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "le_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "le_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Less or equal: " + result + " = " + op1 + " <= " + op2);
    
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
    
    emitInstruction("jle " + trueLabel);
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    emitLabel(endLabel + ":");
}

static void handleGe(TAC* tac) {
    // Greater or equal - use jge
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "ge_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "ge_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Greater or equal: " + result + " = " + op1 + " >= " + op2);
    
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
    
    emitInstruction("jge " + trueLabel);
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    emitLabel(endLabel + ":");
}

static void handleNe(TAC* tac) {
    // Not equal - use jne
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    std::string trueLabel = "ne_true_" + std::to_string(g_labelCounter++);
    std::string endLabel = "ne_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Not equal: " + result + " = " + op1 + " != " + op2);
    
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
    
    emitInstruction("jne " + trueLabel);
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    emitLabel(trueLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    emitLabel(endLabel + ":");
}

static void handleAnd(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Logical AND: " + result + " = " + op1 + " && " + op2);
    
    // Load first operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Load second operand
    if (op2[0] == '$') {
        emitInstruction("movl " + op2 + ", %ebx");
    } else {
        emitInstruction("movl " + op2 + ", %ebx");
    }
    
    // Perform bitwise AND
    emitInstruction("andl %ebx, %eax");
    
    // Store result
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleOr(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    std::string op2 = getOperandValue(tac->getOp2());
    
    std::string resultLoc = allocateVariable(result);
    
    emitComment("Logical OR: " + result + " = " + op1 + " || " + op2);
    
    // Load first operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Load second operand
    if (op2[0] == '$') {
        emitInstruction("movl " + op2 + ", %ebx");
    } else {
        emitInstruction("movl " + op2 + ", %ebx");
    }
    
    // Perform bitwise OR
    emitInstruction("orl %ebx, %eax");
    
    // Store result
    emitInstruction("movl %eax, " + resultLoc);
}

static void handleNot(TAC* tac) {
    std::string result = tac->getRes()->getLexeme();
    std::string op1 = getOperandValue(tac->getOp1());
    
    std::string resultLoc = allocateVariable(result);
    std::string zeroLabel = "not_zero_" + std::to_string(g_labelCounter++);
    std::string endLabel = "not_end_" + std::to_string(g_labelCounter++);
    
    emitComment("Logical NOT: " + result + " = !" + op1);
    
    // Load operand
    if (op1[0] == '$') {
        emitInstruction("movl " + op1 + ", %eax");
    } else {
        emitInstruction("movl " + op1 + ", %eax");
    }
    
    // Compare with zero
    emitInstruction("cmpl $0, %eax");
    emitInstruction("je " + zeroLabel);
    
    // Non-zero case (result = 0)
    emitInstruction("movl $0, " + resultLoc);
    emitInstruction("jmp " + endLabel);
    
    // Zero case (result = 1)
    emitLabel(zeroLabel + ":");
    emitInstruction("movl $1, " + resultLoc);
    
    emitLabel(endLabel + ":");
}

static std::string getOperandLocation(Symbol* symbol) {
    if (!symbol) return "";
    
    std::string name = symbol->getLexeme();
    
    // Check if already allocated
    auto it = g_varLocations.find(name);
    if (it != g_varLocations.end()) {
        return it->second;
    }
    
    // Allocate new location
    return allocateVariable(name);
}

static std::string getOperandValue(Symbol* symbol) {
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

static std::string allocateVariable(const std::string& name, int size) {
    auto it = g_varLocations.find(name);
    if (it != g_varLocations.end()) {
        return it->second;
    }
    
    g_stackOffset += size;
    std::string location = "-" + std::to_string(g_stackOffset) + "(%ebp)";
    g_varLocations[name] = location;
    return location;
}

static std::string allocateVector(const std::string& name, int elements) {
    return allocateVariable(name, elements * 4);  // 4 bytes per element
}

static std::string getStringLabel(const std::string& str) {
    // Find existing string or add new one
    auto it = std::find(g_stringLiterals.begin(), g_stringLiterals.end(), str);
    if (it != g_stringLiterals.end()) {
        size_t index = std::distance(g_stringLiterals.begin(), it);
        return "$str_" + std::to_string(index);
    }
    
    // Add new string
    g_stringLiterals.push_back(str);
    return "$str_" + std::to_string(g_stringLiterals.size() - 1);
}

static void emitInstruction(const std::string& instruction) {
    (*g_outFile) << "\t" << instruction << std::endl;
}

static void emitLabel(const std::string& label) {
    (*g_outFile) << label << std::endl;
}

static void emitComment(const std::string& comment) {
    (*g_outFile) << "\t# " << comment << std::endl;
}

static void generatePrologue() {
    emitComment("Function prologue");
    emitInstruction("pushl %ebp");
    emitInstruction("movl %esp, %ebp");
    
    // Reserve space for local variables if needed
    if (g_stackOffset > 0) {
        emitInstruction("subl $" + std::to_string(g_stackOffset) + ", %esp");
    }
}

static void generateEpilogue() {
    emitComment("Function epilogue");
    emitInstruction("movl %ebp, %esp");
    emitInstruction("popl %ebp");
    emitInstruction("ret");
}

static void saveRegisters() {
    emitComment("Save registers");
    emitInstruction("pushl %eax");
    emitInstruction("pushl %ebx");
    emitInstruction("pushl %ecx");
    emitInstruction("pushl %edx");
}

static void restoreRegisters() {
    emitComment("Restore registers");
    emitInstruction("popl %edx");
    emitInstruction("popl %ecx");
    emitInstruction("popl %ebx");
    emitInstruction("popl %eax");
}

// Public interface function
void generateASM(TAC* tacHead, const std::string& outputFileName) {
    std::ofstream outFile(outputFileName);
    if (!outFile) {
        std::cerr << "Error: Could not open output file " << outputFileName << std::endl;
        return;
    }
    
    // Initialize global state
    g_outFile = &outFile;
    g_varLocations.clear();
    g_vectorSizes.clear();
    g_stringLiterals.clear();
    g_stackOffset = 0;
    g_labelCounter = 0;
    g_stringCounter = 0;
    g_currentRegister = "%eax";
    
    generateAssembly(tacHead);
    
    outFile.close();
    
    std::cout << "Assembly code generated successfully in " << outputFileName << std::endl;
}