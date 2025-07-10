// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File symbol.cpp made by Nathan Guimaraes (334437) on 28/03/2025

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
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
        dataType dataTypeVal = dataType::UNDEFINED;
        identifierType identifierTypeVal = identifierType::UNDEFINED;
        
        switch (type) {
            case LIT_INT:
                dataTypeVal = dataType::INT;
                identifierTypeVal = identifierType::SCALAR;
                break;
            case LIT_REAL:
                dataTypeVal = dataType::REAL;
                identifierTypeVal = identifierType::SCALAR;
                break;
            case LIT_CHAR:
                dataTypeVal = dataType::CHAR;
                identifierTypeVal = identifierType::SCALAR;
                break;
            case LIT_STRING:
                dataTypeVal = dataType::STRING;
                identifierTypeVal = identifierType::LITERAL;
                break;
            case TK_IDENTIFIER:
                // For identifiers, type will be determined later during semantic analysis
                dataTypeVal = dataType::UNDEFINED;
                identifierTypeVal = identifierType::UNDEFINED;
                break;
            default:
                dataTypeVal = dataType::UNDEFINED;
                identifierTypeVal = identifierType::UNDEFINED;
                break;
        }

        Symbol *s = new Symbol(lex, type, dataTypeVal, identifierTypeVal, lineNumber);
        symbolTable[lex] = s;
        return s;
    } else {
        return symbolTable.find(lex)->second;
    }
}

Symbol *getSymbol(string lex) {
    auto it = symbolTable.find(lex);
    if (it != symbolTable.end()) {
        return it->second;
    }
    return nullptr;
}

string invertNumberInt(string number){
    string invertedNum(number.size(), '\0');
    for(unsigned long i = 0; i < number.size(); i++){
        invertedNum[i] = number[number.size() - i - 1];
    }
    return invertedNum;
}

string removeZeros(string number) {
    size_t firstNonZero = number.find_first_not_of('0');
    if (firstNonZero == string::npos) {
        return "0";
    }
    return number.substr(firstNonZero);
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
    case YYerror:
        return "TOKEN_ERROR";
    case INTERNAL:
        return "INTERNAL";
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

string dataTypeName(dataType type) {
    switch (type) {
        case dataType::BYTE:
            return "BYTE";
        case dataType::INT:
            return "INT";
        case dataType::REAL:
            return "REAL";
        case dataType::BOOLEAN:
            return "BOOLEAN";
        case dataType::CHAR:
            return "CHAR";
        case dataType::STRING:
            return "STRING";
        case dataType::VOID:
            return "VOID";
        case dataType::ADDRESS:
            return "ADDRESS";
        case dataType::UNDEFINED:
            return "UNDEFINED";
        default:
            return "UNKNOWN";
    }
}

string identifierTypeName(identifierType type) {
    switch (type) {
        case identifierType::VARIABLE:
            return "VARIABLE";
        case identifierType::VECTOR:
            return "VECTOR";
        case identifierType::FUNCTION:
            return "FUNCTION";
        case identifierType::LITERAL:
            return "LITERAL";
        case identifierType::SCALAR:
            return "SCALAR";
        case identifierType::LABEL:
            return "LABEL";
        case identifierType::TEMP:
            return "TEMP";
        case identifierType::UNDEFINED:
            return "UNDEFINED";
        default:
            return "UNKNOWN";
    }
}

void printSymbolTable() {
    std::cout << "+-------------------------------------------------------------------------------------------+\n";
    std::cout << "|                     SYMBOL TABLE MADE BY NATHAN GUIMARAES (334437)                        |\n";
    std::cout << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
    std::cout << "| " << std::left << std::setw(18) << "LEXEME";
    std::cout << " | " << std::setw(18) << "TYPE";
    std::cout << " | " << std::setw(18) << "DATA TYPE";
    std::cout << " | " << std::setw(18) << "IDENTIFIER TYPE";
    std::cout << " | " << std::setw(5) << "LINE";
    std::cout << " |\n";
    std::cout << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
    
    for (auto &lex : symbolTable){
        std::string lexeme = lex.first;
        std::string type = tokenName(lex.second->getType());
        std::string dataTypeStr = dataTypeName(lex.second->getDataType());
        std::string identifierTypeStr = identifierTypeName(lex.second->getIdentifierType());
        int lineNumber = lex.second->getLineNumber();
        
        // Truncate strings if they are too long (max 17 characters to fit in 18-char column)
        if (lexeme.length() > 17) {
            lexeme = lexeme.substr(0, 14) + "...";
        }
        if (type.length() > 17) {
            type = type.substr(0, 14) + "...";
        }
        if (dataTypeStr.length() > 17) {
            dataTypeStr = dataTypeStr.substr(0, 14) + "...";
        }
        if (identifierTypeStr.length() > 17) {
            identifierTypeStr = identifierTypeStr.substr(0, 14) + "...";
        }
        
        std::cout << "| " << std::left << std::setw(18) << lexeme;
        std::cout << " | " << std::setw(18) << type;
        std::cout << " | " << std::setw(18) << dataTypeStr;
        std::cout << " | " << std::setw(18) << identifierTypeStr;
        std::cout << " | " << std::setw(5) << lineNumber;
        std::cout << " |\n";
    }
    
    std::cout << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
}

void printSymbolTableToFile(const string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        fprintf(stderr, "Error: Could not open file %s for writing symbol table.\n", filename.c_str());
        return;
    }

    outFile << "+-------------------------------------------------------------------------------------------+\n";
    outFile << "|                     SYMBOL TABLE MADE BY NATHAN GUIMARAES (334437)                        |\n";
    outFile << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
    outFile << "| " << std::left << std::setw(18) << "LEXEME";
    outFile << " | " << std::setw(18) << "TYPE";
    outFile << " | " << std::setw(18) << "DATA TYPE";
    outFile << " | " << std::setw(18) << "IDENTIFIER TYPE";
    outFile << " | " << std::setw(5) << "LINE";
    outFile << " |\n";
    outFile << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
    
    for (auto &lex : symbolTable){
        std::string lexeme = lex.first;
        std::string type = tokenName(lex.second->getType());
        std::string dataTypeStr = dataTypeName(lex.second->getDataType());
        std::string identifierTypeStr = identifierTypeName(lex.second->getIdentifierType());
        int lineNumber = lex.second->getLineNumber();
        
        // Truncate strings if they are too long (max 17 characters to fit in 18-char column)
        if (lexeme.length() > 17) {
            lexeme = lexeme.substr(0, 14) + "...";
        }
        if (type.length() > 17) {
            type = type.substr(0, 14) + "...";
        }
        if (dataTypeStr.length() > 17) {
            dataTypeStr = dataTypeStr.substr(0, 14) + "...";
        }
        if (identifierTypeStr.length() > 17) {
            identifierTypeStr = identifierTypeStr.substr(0, 14) + "...";
        }
        
        outFile << "| " << std::left << std::setw(18) << lexeme;
        outFile << " | " << std::setw(18) << type;
        outFile << " | " << std::setw(18) << dataTypeStr;
        outFile << " | " << std::setw(18) << identifierTypeStr;
        outFile << " | " << std::setw(5) << lineNumber;
        outFile << " |\n";
    }
    
    outFile << "+--------------------+--------------------+--------------------+--------------------+-------+\n";
    
    outFile.close();
}