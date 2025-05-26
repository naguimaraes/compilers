// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Abstract Syntax Tree (AST) header file made by Nathan Alonso Guimarães (00334437)

#pragma once
#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <string>
#include <fstream>
#include "symbol.hpp"

enum class ASTNodeType {
    UNKNOWN,
    SYMBOL,

    DECLARATION_LIST,
    VARIABLE_DECLARATION,
    VECTOR_DECLARATION, VECTOR_INITIALIZATION,
    FUNCTION_DECLARATION, PARAMETERS_LIST, FUNCTION_CALL, FORMAL_PARAMETERS,
    COMMAND, COMMAND_LIST,
    ASSIGNMENT,

    ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
    AND, OR, NOT,
    LESS_THAN, GREATER_THAN, LESS_EQUAL, GREATER_EQUAL, EQUAL, DIFFERENT,
    
    BYTE, INT, REAL, LITERAL,
    IF, ELSE, WHILE_DO, DO_WHILE,
    READ, PRINT, RETURN
};

class ASTNode{
private:
    ASTNodeType type;
    vector<ASTNode*> children;
    Symbol* symbol;
public:
    ASTNode(ASTNodeType type = ASTNodeType::UNKNOWN, Symbol* symbol = nullptr, vector<ASTNode*> children = {});

    void addChild(ASTNode* child);

    vector<ASTNode*> getChildren();

    Symbol* getSymbol();

    ASTNodeType getType();

};

void printAST(int level = 0, ASTNode* node = nullptr);

void decompileAST(std::ofstream& outFile, ASTNode* node, int indent = 0);

#endif