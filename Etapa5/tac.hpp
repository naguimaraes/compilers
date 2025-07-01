// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Three Address Code (TAC) header file made by Nathan Alonso Guimarães (00334437)

#pragma once

#include "symbol.hpp"
#include "ast.hpp"
#include <string>

// TAC operation types
enum class TACType {
    SYMBOL,     // Symbol reference (utility)
                // Using res = a, op1 = b, op2 = c
    MOVE,       // Assignment: a = b
    INIT,       // Variable initialization: a = b (first assignment)
    ADD,        // Addition: a = b + c
    SUB,        // Subtraction: a = b - c
    MUL,        // Multiplication: a = b * c
    DIV,        // Division: a = b / c
    MOD,        // Modulo: a = b % c
    AND,        // Logical AND: a = b && c
    OR,         // Logical OR: a = b || c
    NOT,        // Logical NOT: a = !b
    LT,         // Less than: a = b < c
    GT,         // Greater than: a = b > c
    LE,         // Less or equal: a = b <= c
    GE,         // Greater or equal: a = b >= c
    EQ,         // Equal: a = b == c
    NE,         // Not equal: a = b != c
    LABEL,      // Label (utility)
    BEGINFUN,   // Begin function definition
    ENDFUN,     // End function definition
    IFZ,        // Conditional jump if zero: if (a == 0) goto label
    JUMP,       // Unconditional jump: goto label
    CALL,       // Function call: a = call f
    ARG,        // Function argument: arg a
    RET,        // Return: return a
    PRINT,      // Print: print a
    READ,       // Read: read a
    VECWRITE,   // Vector write: a[b] = c
    VECREAD,    // Vector read: a = b[c]
    BEGINVEC,   // Begin vector initialization
    ENDVEC      // End vector initialization
};

// TAC instruction structure
class TAC {
private:
    TACType type;       // Type from TACType enum above
    Symbol* result;     // Result operand
    Symbol* operand1;   // First operand
    Symbol* operand2;   // Second operand
    TAC* previous;      // Previous instruction (for inverted list)
    TAC* next;          // Next instruction (for normal list)

public:
    TAC(TACType type, Symbol* result = nullptr, Symbol* operand1 = nullptr, Symbol* operand2 = nullptr);
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

// Public interface functions
TAC* generateTAC(ASTNode* node);
void printTAC(TAC* tac);
void initTAC();