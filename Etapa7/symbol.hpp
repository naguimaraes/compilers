// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Symbol table header file made by Nathan Alonso Guimarães (00334437)

#pragma once
enum class ASTNodeType; // Forward declaration
#include "ast.hpp"
#include <string>

using namespace std;

// Data types supported by the language
enum class dataType {
    BYTE,       // 8-bit signed integer
    INT,        // 32-bit signed integer
    REAL,       // Number in the format a/b
    BOOLEAN,    // Boolean value (true/false)
    CHAR,       // Single character
    STRING,     // String literal
    VOID,       // Void type (for functions)
    ADDRESS,    // Address type (for labels)
    UNDEFINED   // Undefined/uninitialized type
};

// Identifier types for symbol classification
enum class identifierType {
    VARIABLE,   // Variable identifier
    VECTOR,     // Vector/array identifier
    FUNCTION,   // Function identifier
    LITERAL,    // Literal value
    SCALAR,     // Scalar value
    LABEL,      // Label identifier (for control flow)
    TEMP,       // Temporary variable (compiler-generated)
    UNDEFINED   // Undefined/uninitialized identifier type
};


// Symbol structure - represents an entry in the symbol table
class Symbol {
private:
    int type;                   // Token type from lexical analysis
    string lexeme;              // String representation of the symbol
    dataType data;              // Data type of the symbol
    identifierType identifier;  // Identifier type classification
    int lineNumber;             // Line number where symbol was declared

public:
    Symbol(string lexeme, int type, dataType data = dataType::UNDEFINED, identifierType identifier = identifierType::UNDEFINED, int lineNumber = 1);
    
    // Getters
    string getLexeme();
    int getType();
    dataType getDataType();
    identifierType getIdentifierType();
    int getLineNumber();
    
    // Setters
    void setDataType(dataType data);
    void setIdentifierType(identifierType identifier);
    void setLineNumber(int lineNumber);
};

// Public interface functions - symbol table management
Symbol *insertSymbol(string lex, int type, int lineNumber = 1);    // Insert symbol into symbol table
Symbol *getSymbol(string lex);                                     // Get symbol from symbol table
void printSymbolTable();                                           // Print entire symbol table to stderr
void printSymbolTableToFile(const string& filename);               // Print entire symbol table to file
string invertNumberInt(string number);                             // Convert integer string format
string invertNumberReal(string number);                            // Convert real number string format
string removeZeros(string number);                                 // Remove leading zeros from a number string

/*
    The function to remove zeros on the left from the number has been removed
    from invertNumberInt so that the decompiled code is exactly the same as the original,
    but the zeros are still removed when moving on to TAC generation.
*/