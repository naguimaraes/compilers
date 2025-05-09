#include "ast.hpp"
#include <stdio.h>

vector<string> ASTTypeNames = {
    "UNKNOWN",
    "SYMBOL",

    "DECLARATION_LIST",
    "VARIABLE_DECLARATION",
    "VECTOR_DECLARATION", "VECTOR_INITIALIZATION",
    "FUNCTION_DECLARATION", "PARAMETERS_LIST", "FUNCTION_CALL", "FORMAL_PARAMETERS",
    "COMMAND", "COMMAND_LIST",
    "ASSIGNMENT",

    "ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "MODULO",
    "AND", "OR", "NOT",
    "LESS_THAN", "GREATER_THAN", "LESS_EQUAL", "GREATER_EQUAL", "EQUAL", "DIFFERENT",
    
    "BYTE", "INT", "REAL", "LITERAL",
    "IF", "ELSE", "WHILE_DO", "DO_WHILE",
    "READ", "PRINT", "RETURN",
};

ASTNode::ASTNode(ASTNodeType type, Symbol* symbol, vector<ASTNode*> children)
    : type(type), children(children), symbol(symbol) {} 

void ASTNode::addChild(ASTNode* child) {
    children.push_back(child);
}

vector<ASTNode*> ASTNode::getChildren() {
    return children;
}

Symbol* ASTNode::getSymbol() {
    return symbol;
}

ASTNodeType ASTNode::getType() {
    return type;
}

std::string getIndent(int indent) {
    return std::string(indent, '\t');
}

void printAST(int level, ASTNode* node) {
    if(!node) return;
    string indent(static_cast<size_t>(level * 2), ' ');
    fprintf(stderr, "%s%s", indent.c_str(), ASTTypeNames[static_cast<size_t>(static_cast<int>(node->getType()))].c_str());
    if (node->getSymbol())
        fprintf(stderr, " (symbol: %s)", node->getSymbol()->getLex().c_str());
    fprintf(stderr, "\n");
    for (ASTNode* child : node->getChildren()) {
        printAST(level + 1, child);
    }
    if (node->getType() == ASTNodeType::IF) {
        fprintf(stderr, "%selse\n", indent.c_str());
        for (ASTNode* child : node->getChildren()) {
            printAST(level + 1, child);
        }
    }
}

void decompileAST(std::ofstream& outFile, ASTNode* node, int indent) {
    if (!node) return;
    
    std::string currentIndent = getIndent(indent);
    
    switch (node->getType()) {
        case ASTNodeType::DECLARATION_LIST: {
            for (ASTNode* child : node->getChildren()) {
                decompileAST(outFile, child, indent);
                if (child != node->getChildren().back()) {
                    outFile << "\n";
                }
            }
            break;
        }
        
        case ASTNodeType::VARIABLE_DECLARATION: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " ";
            }
            
            if (node->getChildren().size() > 1 && node->getChildren()[1] && node->getChildren()[1]->getSymbol()) {
                outFile << node->getChildren()[1]->getSymbol()->getLex() << " = ";
            }
            
            if (node->getChildren().size() > 2) {
                decompileAST(outFile, node->getChildren()[2], 0);
                outFile << ";";
            }
            break;
        }
        
        case ASTNodeType::VECTOR_DECLARATION: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " ";
            }
            
            if (node->getChildren().size() > 1 && node->getChildren()[1] && node->getChildren()[1]->getSymbol()) {
                outFile << node->getChildren()[1]->getSymbol()->getLex();
            }
            
            if (node->getChildren().size() > 2 && node->getChildren()[2] && node->getChildren()[2]->getSymbol()) {
                outFile << "[" << node->getChildren()[2]->getSymbol()->getLex() << "]";
            }
            
            if (node->getChildren().size() > 3) {
                outFile << " = ";
                decompileAST(outFile, node->getChildren()[3], 0);
            }
            
            outFile << ";";
            break;
        }
        
        case ASTNodeType::VECTOR_INITIALIZATION: {
            bool first = true;
            for (ASTNode* child : node->getChildren()) {
                if (!first) outFile << ", ";
                first = false;
                decompileAST(outFile, child, 0);
            }
            break;
        }
        
        case ASTNodeType::FUNCTION_DECLARATION: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " ";
            }
            
            if (node->getChildren().size() > 1 && node->getChildren()[1] && node->getChildren()[1]->getSymbol()) {
                outFile << node->getChildren()[1]->getSymbol()->getLex();
            }
            
            outFile << "(";
            if (node->getChildren().size() > 2 && node->getChildren()[2]) {
                decompileAST(outFile, node->getChildren()[2], 0);
            }
            outFile << ")";
            
            if (node->getChildren().size() > 3) {
                outFile << " ";
                decompileAST(outFile, node->getChildren()[3], indent);
            } else {
                outFile << " {\n}\n";
            }
            
            break;
        }
        
        case ASTNodeType::FORMAL_PARAMETERS: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            break;
        }
        
        case ASTNodeType::PARAMETERS_LIST: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " ";
            }
            
            if (node->getChildren().size() > 1 && node->getChildren()[1] && node->getChildren()[1]->getSymbol()) {
                outFile << node->getChildren()[1]->getSymbol()->getLex();
            }
            
            if (node->getChildren().size() > 2) {
                outFile << ", ";
                decompileAST(outFile, node->getChildren()[2], 0);
            }
            break;
        }
        
        case ASTNodeType::COMMAND: {
            outFile << currentIndent << "{\n";
            
            for (ASTNode* child : node->getChildren()) {
                decompileAST(outFile, child, indent + 1);
            }
            
            outFile << currentIndent << "}\n";
            break;
        }
        
        case ASTNodeType::COMMAND_LIST: {
            for (ASTNode* child : node->getChildren()) {
                decompileAST(outFile, child, indent);
            }
            break;
        }
        
        case ASTNodeType::ASSIGNMENT: {
            if (!node->getChildren().empty() && node->getChildren()[0] && node->getChildren()[0]->getSymbol()) {
                outFile << currentIndent << node->getChildren()[0]->getSymbol()->getLex();
                
                if (!node->getChildren()[0]->getChildren().empty()) {
                    outFile << "[";
                    decompileAST(outFile, node->getChildren()[0]->getChildren()[0], 0);
                    outFile << "]";
                }
                
                outFile << " = ";
            }
            
            if (node->getChildren().size() > 1) {
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ";\n";
            }
            break;
        }
        
        case ASTNodeType::IF: {
            outFile << currentIndent << "if (";
            
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            
            outFile << ")";
            
            if (node->getChildren().size() > 1) {
                if (node->getChildren()[1]->getType() == ASTNodeType::COMMAND) {
                    decompileAST(outFile, node->getChildren()[1], indent);
                } else {
                    outFile << "\n";
                    decompileAST(outFile, node->getChildren()[1], indent + 1);
                }
            }
            
            if (node->getChildren().size() > 2) {
                outFile << currentIndent << "else";
                
                if (node->getChildren()[2]->getType() == ASTNodeType::COMMAND) {
                    decompileAST(outFile, node->getChildren()[2], indent);
                } else {
                    outFile << "\n";
                    decompileAST(outFile, node->getChildren()[2], indent + 1);
                }
            }
            break;
        }
        
        case ASTNodeType::ELSE: {
            for (ASTNode* child : node->getChildren()) {
                decompileAST(outFile, child, indent);
            }
            break;
        }
        
        case ASTNodeType::WHILE_DO: {
            outFile << currentIndent << "while";
            
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            
            outFile << " do";
            
            if (node->getChildren().size() > 1) {
                decompileAST(outFile, node->getChildren()[1], indent);
            } else {
                outFile << ";\n";
            }
            break;
        }
        
        case ASTNodeType::DO_WHILE: {
            outFile << currentIndent << "do";
            
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], indent);
            }
            
            outFile << currentIndent << "while";
            
            if (node->getChildren().size() > 1) {
                decompileAST(outFile, node->getChildren()[1], 0);
            }
            
            outFile << ";\n";
            break;
        }
        
        case ASTNodeType::ADD: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " + ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::SUBTRACT: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " - ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::MULTIPLY: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " * ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::DIVIDE: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " / ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::MODULO: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " % ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::LESS_THAN: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " < ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::GREATER_THAN: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " > ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::LESS_EQUAL: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " <= ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::GREATER_EQUAL: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " >= ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::EQUAL: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " == ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::DIFFERENT: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " != ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::AND: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " & ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::OR: {
            if (node->getChildren().size() >= 2) {
                outFile << "(";
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " | ";
                decompileAST(outFile, node->getChildren()[1], 0);
                outFile << ")";
            }
            break;
        }
        
        case ASTNodeType::NOT: {
            if (!node->getChildren().empty()) {
                outFile << "~";
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            break;
        }
        
        case ASTNodeType::FUNCTION_CALL: {
            if (!node->getChildren().empty() && node->getChildren()[0] && node->getChildren()[0]->getSymbol()) {
                outFile << node->getChildren()[0]->getSymbol()->getLex();
            }
            
            outFile << "(";
            
            if (node->getChildren().size() > 1 && node->getChildren()[1]) {
                if (node->getChildren()[1]->getType() == ASTNodeType::COMMAND_LIST) {
                    bool first = true;
                    for (ASTNode* arg : node->getChildren()[1]->getChildren()) {
                        if (!first) outFile << ", ";
                        first = false;
                        decompileAST(outFile, arg, 0);
                    }
                } else {
                    decompileAST(outFile, node->getChildren()[1], 0);
                }
            }
            
            outFile << ")";
            break;
        }
        
        case ASTNodeType::BYTE: {
            outFile << "byte";
            break;
        }
        
        case ASTNodeType::INT: {
            outFile << "int";
            break;
        }
        
        case ASTNodeType::REAL: {
            outFile << "real";
            break;
        }
        
        case ASTNodeType::LITERAL: {
            if (node->getSymbol())
                outFile << node->getSymbol()->getLex();
            break;
        }
        
        case ASTNodeType::SYMBOL: {
            if (node->getSymbol())
                outFile << node->getSymbol()->getLex();
            break;
        }
        
        case ASTNodeType::READ: {
            outFile << currentIndent << "read ";
            
            if (!node->getChildren().empty() && node->getChildren()[0] && node->getChildren()[0]->getSymbol())
                outFile << node->getChildren()[0]->getSymbol()->getLex();
            outFile << ";\n";
            break;
        }
        
        case ASTNodeType::PRINT: {
            outFile << currentIndent << "print ";
            
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            
            outFile << ";\n";
            break;
        }
        
        case ASTNodeType::RETURN: {
            outFile << currentIndent << "return ";
            
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
            }
            
            outFile << ";\n";
            break;
        }
        
        case ASTNodeType::UNKNOWN:
        default:
            outFile << "/* Unsupported or unknown node type */";
    }
}
