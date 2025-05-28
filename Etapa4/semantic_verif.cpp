#include "semantic_verif.hpp"
#include "parser.tab.hpp"
#include <iostream>

bool semanticVerification(ASTNode* root) {
    if (!root) 
        return false; // No AST to verify

    if(checkDeclarations(root))
        return false; // Return false if a redeclaration is found



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
                }
                
                // For identifiers, get their declared data type
                if (tokenType == TK_IDENTIFIER) {
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
        {
            // Comparison and logical operations always return boolean (represented as INT)
            return dataType::INT;
        }

        case ASTNodeType::NOT:
        {
            // NOT operation always returns boolean (represented as INT)
            return dataType::INT;
        }

        case ASTNodeType::FUNCTION_CALL:
        {
            if (!expression->getChildren().empty() && 
                expression->getChildren()[0] && 
                expression->getChildren()[0]->getSymbol()) {
                
                Symbol* functionSymbol = expression->getChildren()[0]->getSymbol();
                return functionSymbol->getDataType();
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
    // BYTE can be promoted to INT or REAL
    // INT can be promoted to REAL
    if (expected == dataType::INT && actual == dataType::BYTE) {
        return true;
    }
    if (expected == dataType::REAL && (actual == dataType::BYTE || actual == dataType::INT)) {
        return true;
    }
    
    return false;
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
bool checkVectorInitialization(ASTNode* vectorDeclaration, ASTNode* sizeNode) {
    if (!vectorDeclaration || vectorDeclaration->getType() != ASTNodeType::VECTOR_DECLARATION) {
        return false;
    }

    // Get vector information
    ASTNodeType vectorType = vectorDeclaration->getChildren()[0]->getType(); // Vector type
    Symbol* vectorSymbol = vectorDeclaration->getChildren()[1]->getSymbol(); // Vector name
    ASTNode* initializationNode = vectorDeclaration->getChildren().size() > 3 ? vectorDeclaration->getChildren()[3] : nullptr;

    // If there's no initialization, it's valid (vector will be uninitialized)
    if (!initializationNode) {
        return true;
    }

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

    // Count initialized elements and check their types
    int initCount = 1;
    ASTNode* currentInit = initializationNode;
    
    while (currentInit && currentInit->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
        // Check each literal in the initialization
        for (ASTNode* child : currentInit->getChildren()) {
            if (child && child->getType() == ASTNodeType::LITERAL) {
                initCount++;
                
                // Check if the type is compatible
                dataType literalType = getExpressionType(child);
                
                // Type compatibility check
                if (!isTypeCompatible(expectedType, literalType)) {
                    fprintf(stderr, "Semantic error: Vector \"%s\" of type %s cannot be initialized with value of type %s.\n", 
                            vectorSymbol->getLexeme().c_str(), 
                            dataTypeToString(expectedType).c_str(),
                            dataTypeToString(literalType).c_str());
                    return false;
                }
            } else if (child && child->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
                // Continue with nested initialization
                currentInit = child;
                break;
            }
        }
        
        // Move to next initialization node if it exists
        if (currentInit->getChildren().size() > 1 && 
            currentInit->getChildren()[1]->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
            currentInit = currentInit->getChildren()[1];
        } else {
            break;
        }
    }

    // Check if the number of initialized elements matches the vector size
    if (initCount != vectorSize) {
        fprintf(stderr, "Semantic error: Vector \"%s\" declared with size %d but initialized with %d elements.\n", 
                vectorSymbol->getLexeme().c_str(), vectorSize, initCount);
        return false;
    }

    return true;
}

bool checkVectorRedeclaration(ASTNode* vectorDeclaration) {
    Symbol* symbol = vectorDeclaration->getChildren()[1]->getSymbol(); // The second child is the name of the vector
    if(symbol == nullptr) return false;
    
    ASTNodeType type = vectorDeclaration->getChildren()[0]->getType(); // The first child is the type of the variable
    ASTNode* initializationNode = vectorDeclaration->getChildren().size() > 3 ? vectorDeclaration->getChildren()[3] : nullptr; // The fourth child is the initialization node, if it exists

    // Check if the symbol is already declared
    if (!(symbol->getIdentifierType() == identifierType::VECTOR)) {
        symbol->setIdentifierType(identifierType::VECTOR); // First encounter, set the identifier type to VECTOR
        symbol->setDataType(convertToDataType(type)); // Set the data type of the vector
    } 
    else {
        // If the identifier type is already set to VECTOR, it means it was redeclared
        fprintf(stderr, "Semantic error: Vector \"%s\" was redeclared.\n", symbol->getLexeme().c_str());
        return true; 
    }

    return false;
}

bool checkVariableRedeclaration(ASTNode* variableDeclaration) {
    Symbol* symbol = variableDeclaration->getChildren()[1]->getSymbol(); // The second child is always the name of the variable
    if(symbol == nullptr) return false;

    ASTNodeType type = variableDeclaration->getChildren()[0]->getType(); // The first child is the type of the variable

    // Check if the symbol is already declared
    if (!(symbol->getIdentifierType() == identifierType::VARIABLE)) {
        symbol->setIdentifierType(identifierType::VARIABLE); // First encounter, set the identifier type to VARIABLE
        symbol->setDataType(convertToDataType(type)); // Set the data type of the variable
    } 
    else {
        // If the identifier type is already set to VARIABLE, it means it was redeclared       
        fprintf(stderr, "Semantic error: Variable \"%s\" was redeclared.\n", symbol->getLexeme().c_str());
        return true; 
    }
    return false;
}

bool checkFunctionRedeclaration(ASTNode* functionDeclaration) {
    Symbol* symbol = functionDeclaration->getChildren()[1]->getSymbol(); // The second child is the name of the function
    if(symbol == nullptr) return false;

    ASTNodeType type = functionDeclaration->getChildren()[0]->getType(); // The first child is the type of the function

    // Check if the symbol is already declared
    if (!(symbol->getIdentifierType() == identifierType::FUNCTION)) {
        symbol->setIdentifierType(identifierType::FUNCTION); // First encounter, set the identifier type to FUNCTION
        symbol->setDataType(convertToDataType(type)); // Set the data type of the function
    } 
    else {
        // If the identifier type is already set to FUNCTION, it means it was redeclared
        fprintf(stderr, "Semantic error: Function \"%s\" was redeclared.\n", symbol->getLexeme().c_str());
        return true;
    }
    return false;
}

bool checkDeclarations(ASTNode* root) {
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; // Skip null children

        switch (child->getType()) {
            case ASTNodeType::VARIABLE_DECLARATION: 
            {
                if (checkVariableRedeclaration(child)) return true;
            }
            break;

            case ASTNodeType::VECTOR_DECLARATION: 
            {
                if (checkVectorRedeclaration(child)) return true;

                ASTNode* sizeNode = child->getChildren()[2]; // The third child is the size of the vector
                if (!checkVectorInitialization(child, sizeNode)) return true; // Return error if initialization is invalid
            }
            break;

            case ASTNodeType::FUNCTION_DECLARATION: 
            {
                if (checkFunctionRedeclaration(child)) return true;
            }
            break;

            // Recursively check children with the same set
            default:
                if (checkDeclarations(child)) return true; // Propagate the error up
            break;
        }
    }
    
    return false; // No redeclared variable found in this subtree
}
