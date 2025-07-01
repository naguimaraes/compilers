// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Three Address Code (TAC) header file made by Nathan Alonso Guimarães (00334437)

#pragma once

#include "symbol.hpp"
#include "ast.hpp"
#include <string>

// TAC operation types
enum class TACType {
    TAC_SYMBOL,     // Symbol reference (utility)
    TAC_MOVE,       // Assignment: a = b
    TAC_ADD,        // Addition: a = b + c
    TAC_SUB,        // Subtraction: a = b - c
    TAC_MUL,        // Multiplication: a = b * c
    TAC_DIV,        // Division: a = b / c
    TAC_MOD,        // Modulo: a = b % c
    TAC_AND,        // Logical AND: a = b && c
    TAC_OR,         // Logical OR: a = b || c
    TAC_NOT,        // Logical NOT: a = !b
    TAC_LT,         // Less than: a = b < c
    TAC_GT,         // Greater than: a = b > c
    TAC_LE,         // Less or equal: a = b <= c
    TAC_GE,         // Greater or equal: a = b >= c
    TAC_EQ,         // Equal: a = b == c
    TAC_NE,         // Not equal: a = b != c
    TAC_LABEL,      // Label (utility)
    TAC_BEGINFUN,   // Begin function definition
    TAC_ENDFUN,     // End function definition
    TAC_IFZ,        // Conditional jump if zero: if (a == 0) goto label
    TAC_JUMP,       // Unconditional jump: goto label
    TAC_CALL,       // Function call: a = call f
    TAC_ARG,        // Function argument: arg a
    TAC_RET,        // Return: return a
    TAC_PRINT,      // Print: print a
    TAC_READ,       // Read: read a
    TAC_VECWRITE,   // Vector write: a[b] = c
    TAC_VECREAD,    // Vector read: a = b[c]
    TAC_BEGINVEC,   // Begin vector initialization
    TAC_ENDVEC      // End vector initialization
};

// TAC instruction structure
class TAC {
private:
    TACType type;
    Symbol* res;    // Result operand
    Symbol* op1;    // First operand
    Symbol* op2;    // Second operand
    TAC* prev;      // Previous instruction (for inverted list)
    TAC* next;      // Next instruction (for normal list)

public:
    TAC(TACType type, Symbol* res = nullptr, Symbol* op1 = nullptr, Symbol* op2 = nullptr);
    ~TAC();
    
    // Getters
    TACType getType() const;
    Symbol* getRes() const;
    Symbol* getOp1() const;
    Symbol* getOp2() const;
    TAC* getPrev() const;
    TAC* getNext() const;
    
    // Setters
    void setPrev(TAC* prev);
    void setNext(TAC* next);
    
    // Print TAC instruction
    void print() const;
};

// Public interface functions - only these are accessible from other files
TAC* generateCode(ASTNode* node);
void tacPrint(TAC* tac);
void initTAC();