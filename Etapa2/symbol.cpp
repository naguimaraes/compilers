// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File symbol.cpp made by Nathan Guimaraes (334437) on 28/03/2025

#include <string>
#include <map>
#include <iostream>
#include "symbol.h"
#include "parser.tab.hpp"

using namespace std;

map<string, Symbol*> symbolTable;

Symbol *insertSymbol(string lex, int type) {
    
    if (symbolTable.find(lex) == symbolTable.end()) {
        
        Symbol *s = new Symbol(lex, type);
        symbolTable[lex] = s;

        return s;
    }
    else{
        return symbolTable.find(lex)->second;
    }
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

void printTable() {
    printf("\n======== Symbol Table ========\n");
    for (auto &lex : symbolTable){
        printf("Lex: %s, Type: %s\n", lex.first.c_str(), tokenName(lex.second->getType()).c_str());
    }
}