#include "semantic_verif.hpp"
#include "parser.tab.hpp"
#include <iostream>

bool semanticVerification(ASTNode* root) {
     // No AST to verify
    if (!root) return false;

    // Return false if a redeclaration is found
    if(checkDeclarations(root)) return false;

    return true; // Return true if no semantic errors are found
}

// ############## Auxiliary functions ##############

// Convert ASTNodeType to dataType
dataType getExpressionType(ASTNode* expression) {
    if (!expression) {
        return dataType::VOID;
    }

    switch (expression->getType()) {
        case ASTNodeType::LITERAL:
        case ASTNodeType::SYMBOL:
        {
            if (expression->getSymbol()) {
                Symbol* symbol = expression->getSymbol();
                int tokenType = symbol->getType();
                
                // For literals, determine type from token type
                if (tokenType == LIT_INT) {
                    return dataType::INT;
                } else if (tokenType == LIT_REAL) {
                    return dataType::REAL;
                } else if (tokenType == LIT_CHAR) {
                    return dataType::BYTE;
                } else if (tokenType == TK_IDENTIFIER) {
                    // For identifiers, get their declared data type
                    return symbol->getDataType();
                }
            }
            break;
        }

        case ASTNodeType::ADD:
        case ASTNodeType::SUBTRACT:
        case ASTNodeType::MULTIPLY:
        case ASTNodeType::DIVIDE:
        case ASTNodeType::MODULO:
        {
            if (expression->getChildren().size() >= 2) {
                dataType leftType = getExpressionType(expression->getChildren()[0]);
                dataType rightType = getExpressionType(expression->getChildren()[1]);
                
                // Type promotion rules: REAL > INT > BYTE
                if (leftType == dataType::REAL || rightType == dataType::REAL) {
                    return dataType::REAL;
                } else if (leftType == dataType::INT || rightType == dataType::INT) {
                    return dataType::INT;
                } else {
                    return dataType::BYTE;
                }
            }
            break;
        }

        case ASTNodeType::LESS_THAN:
        case ASTNodeType::GREATER_THAN:
        case ASTNodeType::LESS_EQUAL:
        case ASTNodeType::GREATER_EQUAL:
        case ASTNodeType::EQUAL:
        case ASTNodeType::DIFFERENT:
        case ASTNodeType::AND:
        case ASTNodeType::OR:
        case ASTNodeType::NOT:
        {
            // Comparison, logical, and NOT operations always return boolean (represented as INT)
            return dataType::INT;
        }

        case ASTNodeType::FUNCTION_CALL:
        {
            if (!expression->getChildren().empty() && 
                expression->getChildren()[0] && 
                expression->getChildren()[0]->getSymbol()) {
                
                return expression->getChildren()[0]->getSymbol()->getDataType();
            }
            break;
        }

        default:
            break;
    }

    return dataType::VOID;
}

// Check if the expected type is compatible with the actual type
bool isTypeCompatible(dataType expected, dataType actual) {
    // Exact match
    if (expected == actual) {
        return true;
    }
    
    // Type promotion rules: values can be promoted but not demoted
    return (expected == dataType::INT && actual == dataType::BYTE) ||
           (expected == dataType::REAL && (actual == dataType::BYTE || actual == dataType::INT));
}

std::string dataTypeToString(dataType type) {
    switch (type) {
        case dataType::BYTE: return "byte";
        case dataType::INT: return "int";
        case dataType::REAL: return "real";
        case dataType::VOID: return "void";
        default: return "unknown";
    }
}

// ############## Verification functions ##############
bool checkCorrectVectorInitialization(ASTNode* vectorDeclaration) {
    // Get vector information
    ASTNodeType vectorType = vectorDeclaration->getChildren()[0]->getType(); // Vector type (first child)
    Symbol* vectorSymbol = vectorDeclaration->getChildren()[1]->getSymbol(); // Vector name (second child)
    ASTNode* sizeNode = vectorDeclaration->getChildren()[2]; // Size node (third child)
    // Initialization node (fourth child, if it exists)
    ASTNode* initializationNode = vectorDeclaration->getChildren().size() > 3 ? vectorDeclaration->getChildren()[3] : nullptr;

    // If there's no initialization, it's valid (vector will be uninitialized)
    if (!initializationNode) return true;

    // Get the declared vector type
    dataType expectedType = convertToDataType(vectorType);
    
    // Get the size of the vector
    int vectorSize = 0;
    if (sizeNode && sizeNode->getSymbol() && sizeNode->getSymbol()->getType() == LIT_INT) {
        vectorSize = std::stoi(sizeNode->getSymbol()->getLexeme());
    } else {
        fprintf(stderr, "Semantic error: Cannot determine vector size for initialization check.\n");
        return false;
    }

    // Count initialized elements by traversing the nested structure
    int initCount = 0;
    ASTNode* currentInit = initializationNode;
    
    // Traverse down the nested VECTOR_INITIALIZATION structure
    while (currentInit && currentInit->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
        // Each VECTOR_INITIALIZATION should have at least one child (the LITERAL)
        if (currentInit->getChildren().empty()) {
            break;
        }
        
        // Find the LITERAL in this VECTOR_INITIALIZATION node
        ASTNode* literal = nullptr;
        ASTNode* nextInit = nullptr;
        
        for (ASTNode* child : currentInit->getChildren()) {
            if (child && child->getType() == ASTNodeType::LITERAL) {
                literal = child;
            } else if (child && child->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
                nextInit = child;
            }
        }
        
        // If we found a literal, count it and check its type
        if (literal) {
            initCount++;
            
            // Check if the type is compatible
            dataType literalType = getExpressionType(literal);
            
            // Type compatibility check
            if (!isTypeCompatible(expectedType, literalType)) {
                fprintf(stderr, "Semantic error: Vector \"%s\" of type %s cannot be initialized with value of type %s.\n", 
                        vectorSymbol->getLexeme().c_str(), 
                        dataTypeToString(expectedType).c_str(),
                        dataTypeToString(literalType).c_str());
                return false;
            }
        }
        
        // Move to the next nested VECTOR_INITIALIZATION
        currentInit = nextInit;
    }

    // Check if the number of initialized elements matches the vector size
    if (initCount != vectorSize) {
        fprintf(stderr, "Semantic error: Vector \"%s\" declared with size %d but initialized with %d elements.\n", 
                vectorSymbol->getLexeme().c_str(), vectorSize, initCount);
        return false;
    }

    return true;
}

bool checkIdentifierRedeclaration(ASTNode* declaration, identifierType idType, const char* typeName) {
    Symbol* symbol = declaration->getChildren()[1]->getSymbol(); // The second child is the name
    if(symbol == nullptr) return false;

    ASTNodeType type = declaration->getChildren()[0]->getType(); // The first child is the type

    // Check if the symbol is already declared
    if (symbol->getIdentifierType() != idType) {
        symbol->setIdentifierType(idType); // First encounter, set the identifier type
        symbol->setDataType(convertToDataType(type)); // Set the data type
        return false;
    } else {
        // Already declared
        fprintf(stderr, "Semantic error: %s \"%s\" was redeclared.\n", typeName, symbol->getLexeme().c_str());
        return true;
    }
}

bool checkVectorRedeclaration(ASTNode* vectorDeclaration) {
    return checkIdentifierRedeclaration(vectorDeclaration, identifierType::VECTOR, "Vector");
}

bool checkVariableRedeclaration(ASTNode* variableDeclaration) {
    return checkIdentifierRedeclaration(variableDeclaration, identifierType::VARIABLE, "Variable");
}

bool checkFunctionRedeclaration(ASTNode* functionDeclaration) {
    return checkIdentifierRedeclaration(functionDeclaration, identifierType::FUNCTION, "Function");
}

bool checkDeclarations(ASTNode* root) {
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; // Skip null children

        switch (child->getType()) {
            case ASTNodeType::VARIABLE_DECLARATION: 
                if (checkVariableRedeclaration(child)) return true;
                break;

            case ASTNodeType::VECTOR_DECLARATION: 
                if (checkVectorRedeclaration(child) || !checkCorrectVectorInitialization(child)) return true;
                break;

            case ASTNodeType::FUNCTION_DECLARATION: 
                if (checkFunctionRedeclaration(child)) return true;
                break;

            // Recursively check children with the same set
            default:
                if (checkDeclarations(child)) return true; // Propagate the error up
                break;
        }
    }
    
    return false; // No redeclared variable found in this subtree
}
