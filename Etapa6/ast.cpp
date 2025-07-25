// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Abstract Syntax Tree (AST) source code made by Nathan Alonso Guimarães (00334437)

#include "ast.hpp"
#include "symbol.hpp"
#include "parser.tab.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <functional>

std::vector<std::string> ASTTypeNames = {
    "UNKNOWN",
    "SYMBOL",
    
    "PROGRAM",
    "DECLARATION_LIST",
    "VARIABLE_DECLARATION",
    "VECTOR_DECLARATION", "VECTOR_INITIALIZATION", "VECTOR_ACCESS",
    "EXPRESSION_LIST", "ARGUMENTS_LIST", "FUNCTION_DECLARATION", "PARAMETERS_LIST", "FUNCTION_CALL", "FORMAL_PARAMETERS",
    "COMMAND", "COMMAND_LIST",
    "ASSIGNMENT",

    "ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "MODULO",
    "AND", "OR", "NOT",
    "LESS_THAN", "GREATER_THAN", "LESS_EQUAL", "GREATER_EQUAL", "EQUAL", "DIFFERENT",

    "BYTE", "INT", "REAL", "CHAR", "LITERAL",
    "IF", "ELSE", "WHILE_DO", "DO_WHILE",
    "READ", "PRINT", "RETURN",
};

ASTNode::ASTNode(ASTNodeType type, Symbol* symbol, std::vector<ASTNode*> children, int lineNumber)
    : type(type), children(children), symbol(symbol), lineNumber(lineNumber) {} 

void ASTNode::addChild(ASTNode* child) {
    children.push_back(child);
}

std::vector<ASTNode*> ASTNode::getChildren() {
    return this->children;
}

Symbol* ASTNode::getSymbol() {
    return this->symbol;
}

ASTNodeType ASTNode::getType() {
    return this->type;
}

int ASTNode::getLineNumber() const {
    return this->lineNumber;
}

void ASTNode::setLineNumber(int line) {
    this->lineNumber = line;
}

std::string getIndent(int indent) {
    return std::string(indent, '\t');
}

void printAST(int level, ASTNode* node) {
    if(!node) return;
    std::string indent(static_cast<size_t>(level * 2), ' ');
    fprintf(stderr, "%s%s", indent.c_str(), ASTTypeNames[static_cast<size_t>(static_cast<int>(node->getType()))].c_str());
    if (node->getSymbol())
        fprintf(stderr, " (symbol: %s)", node->getSymbol()->getLexeme().c_str());
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

static void printASTToFileRecursive(std::ofstream& outFile, int level, ASTNode* node) {
    if(!node) return;
    std::string indent(static_cast<size_t>(level * 2), ' ');
    outFile << indent << ASTTypeNames[static_cast<size_t>(static_cast<int>(node->getType()))];
    if (node->getSymbol())
        outFile << " (symbol: " << node->getSymbol()->getLexeme() << ")";
    outFile << "\n";
    for (ASTNode* child : node->getChildren()) {
        printASTToFileRecursive(outFile, level + 1, child);
    }
    if (node->getType() == ASTNodeType::IF) {
        outFile << indent << "else\n";
        for (ASTNode* child : node->getChildren()) {
            printASTToFileRecursive(outFile, level + 1, child);
        }
    }
}

void printASTToFile(const std::string& filename, ASTNode* node) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        fprintf(stderr, "Error: Could not open file %s for writing AST.\n", filename.c_str());
        return;
    }

    outFile << "-------------- AST STRUCTURE MADE BY NATHAN GUIMARAES (334437) --------------\n";
    printASTToFileRecursive(outFile, 0, node);
    
    outFile.close();
}

void decompileAST(std::ofstream& outFile, ASTNode* node, int indent) {
    if (!node) return;
    
    std::string currentIndent = getIndent(indent);
    
    switch (node->getType()) {

        case ASTNodeType::PROGRAM: {
            outFile << "// Decompiled 2025++1 code from AST" << std::endl;
            outFile << "// Made by Nathan Guimaraes (334437)\n" << std::endl;
            for (ASTNode* child : node->getChildren()) {
                decompileAST(outFile, child, indent);
            }
            break;
        }

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
                outFile << node->getChildren()[1]->getSymbol()->getLexeme() << " = ";
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
                outFile << node->getChildren()[1]->getSymbol()->getLexeme();
            }
            
            if (node->getChildren().size() > 2 && node->getChildren()[2] && node->getChildren()[2]->getSymbol()) {
                outFile << "[" << invertNumberInt(node->getChildren()[2]->getSymbol()->getLexeme()) << "]";
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
        
        case ASTNodeType::VECTOR_ACCESS: {
            if (!node->getChildren().empty() && node->getChildren()[0] && node->getChildren()[0]->getSymbol()) {
                outFile << node->getChildren()[0]->getSymbol()->getLexeme();
            }
            
            outFile << "[";
            
            if (node->getChildren().size() > 1) {
                decompileAST(outFile, node->getChildren()[1], 0);
            }
            
            outFile << "]";
            break;
        }
        
        case ASTNodeType::FUNCTION_DECLARATION: {
            if (!node->getChildren().empty()) {
                decompileAST(outFile, node->getChildren()[0], 0);
                outFile << " ";
            }
            
            if (node->getChildren().size() > 1 && node->getChildren()[1] && node->getChildren()[1]->getSymbol()) {
                outFile << node->getChildren()[1]->getSymbol()->getLexeme();
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
                outFile << node->getChildren()[1]->getSymbol()->getLexeme();
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
        
        case ASTNodeType::EXPRESSION_LIST: {
            bool first = true;
            for (ASTNode* child : node->getChildren()) {
                if (!first) outFile << ", ";
                first = false;
                decompileAST(outFile, child, 0);
            }
            break;
        }
        
        case ASTNodeType::ARGUMENTS_LIST: {
            bool first = true;
            for (ASTNode* child : node->getChildren()) {
                if (!first) outFile << ", ";
                first = false;
                decompileAST(outFile, child, 0);
            }
            break;
        }
        
        case ASTNodeType::ASSIGNMENT: {
            if (!node->getChildren().empty()) {
                outFile << currentIndent;
                
                // Check if it's a vector assignment
                if (node->getChildren()[0] && node->getChildren()[0]->getType() == ASTNodeType::VECTOR_ACCESS) {
                    decompileAST(outFile, node->getChildren()[0], 0);
                } else if (node->getChildren()[0] && node->getChildren()[0]->getSymbol()) {
                    outFile << node->getChildren()[0]->getSymbol()->getLexeme();
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
                outFile << node->getChildren()[0]->getSymbol()->getLexeme();
            }
            
            outFile << "(";
            
            if (node->getChildren().size() > 1 && node->getChildren()[1]) {
                decompileAST(outFile, node->getChildren()[1], 0);
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
            if (node->getSymbol()) {
                Symbol* symbol = node->getSymbol();
                int type = symbol->getType();


                if (type == LIT_INT) {
                    outFile << invertNumberInt(symbol->getLexeme());
                } else if (type == LIT_REAL) {
                    outFile << invertNumberReal(symbol->getLexeme());
                } else {
                    outFile << symbol->getLexeme();
                }
            }
            break;
        }
        
        case ASTNodeType::SYMBOL: {
            if (node->getSymbol()){
                Symbol* symbol = node->getSymbol();
                int type = symbol->getType();

                if (type == LIT_INT) {
                    outFile << invertNumberInt(symbol->getLexeme());
                } else if (type == LIT_REAL) {
                    outFile << invertNumberReal(symbol->getLexeme());
                } else {
                    outFile << symbol->getLexeme();
                }
            }
            break;
        }
        
        case ASTNodeType::READ: {
            outFile << currentIndent << "read ";
            
            if (!node->getChildren().empty() && node->getChildren()[0] && node->getChildren()[0]->getSymbol())
                outFile << node->getChildren()[0]->getSymbol()->getLexeme();
            outFile << ";\n";
            break;
        }
        
        case ASTNodeType::PRINT: {
            outFile << currentIndent << "print ";
            
            // Recursive function to handle the chained structure correctly
            std::function<void(ASTNode*)> handlePrintElement = [&](ASTNode* node) {
                if (!node) return;
                
                if (node->getType() == ASTNodeType::LITERAL && node->getSymbol()) {
                    outFile << node->getSymbol()->getLexeme() << " ";
                    // Process any children of this literal
                    for (ASTNode* child : node->getChildren()) {
                        handlePrintElement(child);
                    }
                } else if (node->getType() == ASTNodeType::ADD || node->getType() == ASTNodeType::SUBTRACT || 
                          node->getType() == ASTNodeType::MULTIPLY || node->getType() == ASTNodeType::DIVIDE) {
                    // For operations, decompile them and then check for more chained elements
                    decompileAST(outFile, node, 0);
                    outFile << " ";
                    // Check if there are additional elements chained after this operation
                    for (ASTNode* child : node->getChildren()) {
                        if (child->getType() == ASTNodeType::LITERAL) {
                            handlePrintElement(child);
                        }
                    }
                } else if (node->getType() == ASTNodeType::SYMBOL) {
                    decompileAST(outFile, node, 0);
                    outFile << " ";
                    // Process any children
                    for (ASTNode* child : node->getChildren()) {
                        handlePrintElement(child);
                    }
                } else if (node->getType() == ASTNodeType::VECTOR_ACCESS) {
                    decompileAST(outFile, node, 0);
                    outFile << " ";
                    // For VECTOR_ACCESS, only process LITERAL children that are not part of the access itself
                    // Skip the first two children (vector name and index) and only process additional literals
                    for (size_t i = 2; i < node->getChildren().size(); i++) {
                        if (node->getChildren()[i]->getType() == ASTNodeType::LITERAL) {
                            handlePrintElement(node->getChildren()[i]);
                        }
                    }
                }
            };
            
            // Process all children of the PRINT node
            for (ASTNode* child : node->getChildren()) {
                handlePrintElement(child);
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
