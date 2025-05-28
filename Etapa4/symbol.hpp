// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File symbol.h made by Nathan Guimaraes (334437) on 28/03/2025

#pragma once
enum class ASTNodeType; // Forward declaration
#include "ast.hpp"
#include <string>

using namespace std;

enum class dataType {
    BYTE, INT, REAL, BOOLEAN, CHAR, STRING, VOID, UNDEFINED
};

enum class identifierType {
    VARIABLE, VECTOR, FUNCTION, LITERAL, UNDEFINED
};


class Symbol {
    private:
        int type;
        string lexeme;
        dataType data;
        identifierType identifier;
    public:
        Symbol(string lexeme, int type, dataType data = dataType::UNDEFINED, identifierType identifier = identifierType::UNDEFINED);  
        string getLexeme();
        int getType();
        dataType getDataType();
        void setDataType(dataType data);
        identifierType getIdentifierType();
        void setIdentifierType(identifierType identifier);
};

Symbol *insertSymbol(string lex, int type);
void printSymbolTable();
dataType convertToDataType(int type);
dataType convertToDataType(ASTNodeType typeName);
string invertNumberInt(string number);
string invertNumberReal(string number);