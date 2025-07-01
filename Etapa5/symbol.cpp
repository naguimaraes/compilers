// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File symbol.cpp made by Nathan Guimaraes (334437) on 28/03/2025

#include <string>
#include <map>
#include <iostream>
#include "symbol.hpp"
#include "parser.tab.hpp"

using namespace std;

map<string, Symbol*> symbolTable;

Symbol::Symbol(string lexeme, int type, dataType data, identifierType identifier, int lineNumber){
    this->lexeme = lexeme;
    this->type = type;
    this->data = data;
    this->identifier = identifier;
    this->lineNumber = lineNumber;
}

string Symbol::getLexeme() {
    return this->lexeme;
}

int Symbol::getType() {
    return this->type;
}

dataType Symbol::getDataType() {
    return this->data;
}

identifierType Symbol::getIdentifierType() {
    return this->identifier;
}

void Symbol::setDataType(dataType data) {
    this->data = data;
}

void Symbol::setIdentifierType(identifierType identifier) {
    this->identifier = identifier;
}

int Symbol::getLineNumber() {
    return this->lineNumber;
}

void Symbol::setLineNumber(int lineNumber) {
    this->lineNumber = lineNumber;
}

Symbol *insertSymbol(string lex, int type, int lineNumber) {

    if (symbolTable.find(lex) == symbolTable.end()) {

        Symbol *s = new Symbol(lex, type, dataType::UNDEFINED, identifierType::UNDEFINED, lineNumber);
        symbolTable[lex] = s;

        return s;
    }
    else{
        return symbolTable.find(lex)->second;
    }
}

string invertNumberInt(string number){
        
    string invertedNum(number.size(), '\0');

    for(unsigned long i = 0; i < number.size(); i++){
        invertedNum[i] = number[number.size() - i - 1];
    }

    // Remove leading zeros
    size_t firstNonZero = invertedNum.find_first_not_of('0');
    if (firstNonZero == string::npos) {
        return "0"; // All zeros case
    }
    return invertedNum.substr(firstNonZero);
}

string invertNumberReal(string number){
    unsigned long i = 0;

    while(number[i] != '/'){
        i++;
    }

    return invertNumberInt(number.substr(0, i)) + "/" + invertNumberInt(number.substr(i + 1, number.size() - i - 1));        
}

string tokenName(int token){

    switch (token){
    case KW_BYTE:
        return "KW_BYTE";
    case KW_INT:
        return "KW_INT";
    case KW_REAL:
        return "KW_REAL";
    case KW_IF:
        return "KW_IF";
    case KW_ELSE:
        return "KW_ELSE";
    case KW_DO:
        return "KW_DO";
    case KW_WHILE:
        return "KW_WHILE";
    case KW_READ:
        return "KW_READ";
    case KW_PRINT:
        return "KW_PRINT";
    case KW_RETURN:
        return "KW_RETURN";
    case OPERATOR_LE:
        return "OPERATOR_LE";
    case OPERATOR_GE:
        return "OPERATOR_GE";
    case OPERATOR_EQ:
        return "OPERATOR_EQ";
    case OPERATOR_DIF:
        return "OPERATOR_DIF";
    case TK_IDENTIFIER:
        return "TK_IDENTIFIER";
    case LIT_INT:
        return "LIT_INT";
    case LIT_CHAR:
        return "LIT_CHAR";
    case LIT_REAL:
        return "LIT_REAL";
    case LIT_STRING:
        return "LIT_STRING";
    case TOKEN_ERROR:
        return "TOKEN_ERROR";
    case '-':
    case ',':
    case ';':
    case ':':
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '+':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '&':
    case '|':
    case '~':
    case '=':
    case '!':
        return std::string(1, static_cast<char>(token));
    default:
        return "UNKNOWN";
    }
}

void printSymbolTable() {
    printf("\n======== Symbol Table ========\n");
    for (auto &lex : symbolTable){
        printf("Lex: %s, Type: %s\n", lex.first.c_str(), tokenName(lex.second->getType()).c_str());
    }
}