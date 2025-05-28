// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Abstract Syntax Tree (AST) header file made by Nathan Alonso Guimarães (00334437)

#pragma once

#include <vector>
#include <string>
#include <fstream>

// Forward declaration
class Symbol;

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
    READ, PRINT, RETURN,
};

extern std::vector<std::string> ASTTypeNames;

class ASTNode {
private:
    ASTNodeType type;
    std::vector<ASTNode*> children;
    Symbol* symbol;

public:
    ASTNode(ASTNodeType type = ASTNodeType::UNKNOWN, Symbol* symbol = nullptr, std::vector<ASTNode*> children = {});
    void addChild(ASTNode* child);
    std::vector<ASTNode*> getChildren();
    Symbol* getSymbol();
    ASTNodeType getType();
};

void printAST(int level, ASTNode* node);
void decompileAST(std::ofstream& outFile, ASTNode* node, int depth);