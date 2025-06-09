#include "semantic_verif.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <functional>

// ############## Auxiliary functions ##############

// Convert ASTNodeType to dataType
dataType convertToDataType(ASTNodeType typeName) {
    switch (typeName) {
        case ASTNodeType::BYTE:
            return dataType::BYTE;
        case ASTNodeType::INT:
            return dataType::INT;
        case ASTNodeType::REAL:
            return dataType::REAL;
        default:
            return dataType::VOID;
    }
}

// Convert token type to dataType
dataType convertToDataType(int type) {
    switch (type) {
        case KW_BYTE:
            return dataType::BYTE;
        case KW_INT:
            return dataType::INT;
        case KW_REAL:
            return dataType::REAL;
        default:
            return dataType::VOID;
    }
}

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
                    return dataType::CHAR;
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

    // Rules:
    // INT and CHAR are compatible
    if (expected == dataType::INT && actual == dataType::CHAR) {
        return true;
    }
    if (expected == dataType::CHAR && actual == dataType::INT) {
        return true;
    }
    
    //All other combinations are incompatible
    return false;
}

const char* dataTypeToString(dataType type) {
    switch (type) {
        case dataType::BYTE:
            return "byte";
        case dataType::INT:
            return "int";
        case dataType::REAL:
            return "real";
        case dataType::VOID:
            return "void";
        case dataType::CHAR:
            return "char";
        case dataType::STRING:
            return "string";
        default:
            return "unknown";
    }
}

// ############## Verification functions ##############

// If the vector is correctly initialized, this function will return true
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
                fprintf(stderr, "Semantic error at line %d: Vector \"%s\" of type %s cannot be initialized with value of type %s.\n", 
                        literal->getSymbol()->getLineNumber(),
                        vectorSymbol->getLexeme().c_str(), 
                        dataTypeToString(expectedType),
                        dataTypeToString(literalType));
                return false;
            }
        }
        
        // Move to the next nested VECTOR_INITIALIZATION
        currentInit = nextInit;
    }

    // Check if the number of initialized elements matches the vector size
    if (initCount != vectorSize) {
        fprintf(stderr, "Semantic error at line %d: Vector \"%s\" declared with size %d but initialized with %d elements.\n", 
                vectorSymbol->getLineNumber(), vectorSymbol->getLexeme().c_str(), vectorSize, initCount);
        return false;
    }

    return true;
}

// If the identifier is already declared, this function will return the number of errors (0 or 1)
int checkRedeclaration(ASTNode* declaration, identifierType expectedIdentifierType, const char* typeName) {
    Symbol* symbol = declaration->getChildren()[1]->getSymbol(); // The second child is the name of the identifier
    if(symbol == nullptr) return 0;

    ASTNodeType type = declaration->getChildren()[0]->getType(); // The first child is the type of the identifier

    // Check if the symbol is already declared
    if (symbol->getIdentifierType() == identifierType::UNDEFINED) {
        // First encounter, set the identifier type and data type
        symbol->setIdentifierType(expectedIdentifierType);
        symbol->setDataType(convertToDataType(type));
        return 0;
    } else {
        // Already declared - report error using the line number stored in the symbol
        // This line number corresponds to the redeclaration line, not the first declaration
        int firstDeclarationLine = symbol->getLineNumber();
        
        // We need to report this as a redeclaration. However, the symbol's line number
        // is from the first encounter. We need to find a way to get the current line.
        // For now, we'll create a temporary approach by examining if we can get line info
        // from the literal or other children of the declaration
        int currentDeclarationLine = firstDeclarationLine;
        
        // Try to get line number from the literal (third child) if it exists
        if (declaration->getChildren().size() > 2 && 
            declaration->getChildren()[2] && 
            declaration->getChildren()[2]->getSymbol()) {
            currentDeclarationLine = declaration->getChildren()[2]->getSymbol()->getLineNumber();
        }
        
        fprintf(stderr, "Semantic error at line %d: %s \"%s\" was redeclared (first declared at line %d).\n", 
                currentDeclarationLine, typeName, symbol->getLexeme().c_str(), firstDeclarationLine);
        return 1;
    }
}

// If the function has problems with parameters, this function will return the number of errors
int checkParameters(ASTNode* functionDeclaration) {
    // Get the function name
    Symbol* functionSymbol = functionDeclaration->getChildren()[1]->getSymbol();
    if (!functionSymbol) return 0;

    int errorCount = 0;

    // Check if the function has parameters
    ASTNode* formalParameters = functionDeclaration->getChildren().size() > 2 ? functionDeclaration->getChildren()[2] : nullptr;

    if (!formalParameters || formalParameters->getType() != ASTNodeType::FORMAL_PARAMETERS) {
        return 0; // No parameters or invalid parameter node
    }

    // Get the first PARAMETERS_LIST node
    ASTNode* currentParamList = nullptr;
    for (ASTNode* child : formalParameters->getChildren()) {
        if (child && child->getType() == ASTNodeType::PARAMETERS_LIST) {
            currentParamList = child;
            break;
        }
    }

    // Traverse the nested PARAMETERS_LIST structure
    while (currentParamList && currentParamList->getType() == ASTNodeType::PARAMETERS_LIST) {
        auto children = currentParamList->getChildren();
        
        // Each PARAMETERS_LIST should have: type, symbol, and optionally another PARAMETERS_LIST
        if (children.size() < 2) break;
        
        ASTNode* typeNode = nullptr;
        ASTNode* symbolNode = nullptr;
        ASTNode* nextParamList = nullptr;

        // Find type, symbol, and next parameter list
        for (ASTNode* child : children) {
            if (!child) continue;
            
            if (child->getType() == ASTNodeType::INT || 
                child->getType() == ASTNodeType::BYTE || 
                child->getType() == ASTNodeType::REAL) {
                typeNode = child;
            } else if (child->getType() == ASTNodeType::SYMBOL) {
                symbolNode = child;
            } else if (child->getType() == ASTNodeType::PARAMETERS_LIST) {
                nextParamList = child;
            }
        }
        
        // If we found both type and symbol, process this parameter
        if (typeNode && symbolNode && symbolNode->getSymbol()) {
            Symbol* paramSymbol = symbolNode->getSymbol();
            
            // Check if parameter is already declared
            if (paramSymbol->getIdentifierType() != identifierType::UNDEFINED) {
                fprintf(stderr, "Semantic error at line %d: Parameter \"%s\" was redeclared.\n", 
                        paramSymbol->getLineNumber(), paramSymbol->getLexeme().c_str());
                errorCount++;
            } else {
                // Initialize the parameter
                paramSymbol->setIdentifierType(identifierType::VARIABLE);
                paramSymbol->setDataType(convertToDataType(typeNode->getType()));
            }
        }

        // Move to the next nested PARAMETERS_LIST
        currentParamList = nextParamList;
    }

    return errorCount;
}

// If there are problems with declarations, this function will return the number of errors
int checkDeclarations(ASTNode* root) {
    int errorCount = 0;
    
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; // Skip null children

        switch (child->getType()) {
            case ASTNodeType::VARIABLE_DECLARATION: 
                errorCount += checkRedeclaration(child, identifierType::VARIABLE, "Variable");
                break;

            case ASTNodeType::VECTOR_DECLARATION: 
                errorCount += checkRedeclaration(child, identifierType::VECTOR, "Vector");
                if (!checkCorrectVectorInitialization(child)) errorCount++;
                break;

            case ASTNodeType::FUNCTION_DECLARATION:
                errorCount += checkRedeclaration(child, identifierType::FUNCTION, "Function");
                errorCount += checkParameters(child);
                break;

            // Recursively check children
            default:
                errorCount += checkDeclarations(child);
                break;
        }
    }
    
    return errorCount;
}

// If there are undeclared identifiers, this function will return the number of errors
int checkUndeclaredIdentifiers(ASTNode* root) {
    int errorCount = 0;
    
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; // Skip null children

        if(child->getType() == ASTNodeType::SYMBOL) {
            if(child->getSymbol()->getDataType() == dataType::UNDEFINED) {
                fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is undeclared.\n", 
                        child->getSymbol()->getLineNumber(), child->getSymbol()->getLexeme().c_str());
                errorCount++;
            }
        }
        // Recursively check children
        errorCount += checkUndeclaredIdentifiers(child);
    }

    return errorCount;
}

// Helper function to check return statements in a function body
int checkReturnStatementsInFunction(ASTNode* functionBody, dataType expectedReturnType, const std::string& functionName, int functionLine) {
    if (!functionBody) return 0;

    int errorCount = 0;

    // Traverse all children of the function body
    for (ASTNode* child : functionBody->getChildren()) {
        if (!child) continue;

        switch (child->getType()) {
            case ASTNodeType::RETURN:
            {
                // Check if the return statement has an expression
                if (!child->getChildren().empty() && child->getChildren()[0]) {
                    ASTNode* returnExpression = child->getChildren()[0];
                    dataType actualReturnType = getExpressionType(returnExpression);
                    
                    // Check if the return type is compatible with the declared function type
                    if (!isTypeCompatible(expectedReturnType, actualReturnType)) {
                        fprintf(stderr, "Semantic error at line %d: Function \"%s\" declared as %s but returns %s.\n",
                                functionLine,
                                functionName.c_str(),
                                dataTypeToString(expectedReturnType),
                                dataTypeToString(actualReturnType));
                        errorCount++;
                    }
                } else {
                    // Return statement without expression (void return)
                    if (expectedReturnType != dataType::VOID) {
                        fprintf(stderr, "Semantic error at line %d: Function \"%s\" declared as %s but has void return.\n",
                                functionLine,
                                functionName.c_str(),
                                dataTypeToString(expectedReturnType));
                        errorCount++;
                    }
                }
                break;
            }
            
            // Recursively check nested blocks and control structures
            case ASTNodeType::COMMAND:
            case ASTNodeType::COMMAND_LIST:
            case ASTNodeType::IF:
            case ASTNodeType::ELSE:
            case ASTNodeType::WHILE_DO:
            case ASTNodeType::DO_WHILE:
            {
                errorCount += checkReturnStatementsInFunction(child, expectedReturnType, functionName, functionLine);
                break;
            }
            
            default:
                // For other node types, recursively check their children
                errorCount += checkReturnStatementsInFunction(child, expectedReturnType, functionName, functionLine);
                break;
        }
    }

    return errorCount;
}

// If there are problems with function return types, this function will return the number of errors
int checkFunctionReturnTypes(ASTNode* root) {
    if (!root) return 0;

    int errorCount = 0;

    for (ASTNode* child : root->getChildren()) {
        if (!child) continue;

        switch (child->getType()) {
            case ASTNodeType::FUNCTION_DECLARATION:
            {
                // Get function information
                if (child->getChildren().size() < 2) break;
                
                ASTNode* returnTypeNode = child->getChildren()[0]; // Function return type
                ASTNode* functionNameNode = child->getChildren()[1]; // Function name
                ASTNode* functionBody = child->getChildren().size() > 3 ? child->getChildren()[3] : nullptr; // Function body
                
                if (!returnTypeNode || !functionNameNode || !functionNameNode->getSymbol()) break;
                
                dataType expectedReturnType = convertToDataType(returnTypeNode->getType());
                std::string functionName = functionNameNode->getSymbol()->getLexeme();
                
                // Get the line number from the function declaration itself
                // We need to find the line number where the function is actually declared
                // Try to get it from any child node that has a symbol with line information
                int functionLine = functionNameNode->getSymbol()->getLineNumber();
                
                // Try to get a more accurate line number from the function body or parameters
                if (child->getChildren().size() > 2 && child->getChildren()[2]) {
                    // Check formal parameters for line info
                    ASTNode* formalParams = child->getChildren()[2];
                    if (formalParams->getType() == ASTNodeType::FORMAL_PARAMETERS) {
                        // Look for parameter symbols that might have the correct line
                        std::function<int(ASTNode*)> findLatestLine = [&](ASTNode* node) -> int {
                            int latestLine = functionLine;
                            if (!node) return latestLine;
                            
                            if (node->getSymbol() && node->getSymbol()->getLineNumber() > latestLine) {
                                latestLine = node->getSymbol()->getLineNumber();
                            }
                            
                            for (ASTNode* childNode : node->getChildren()) {
                                int childLine = findLatestLine(childNode);
                                if (childLine > latestLine) {
                                    latestLine = childLine;
                                }
                            }
                            return latestLine;
                        };
                        
                        int paramLine = findLatestLine(formalParams);
                        if (paramLine > functionLine) {
                            functionLine = paramLine;
                        }
                    }
                }
                
                // If we still have the original line, try to get line from function body
                if (functionBody && functionLine == functionNameNode->getSymbol()->getLineNumber()) {
                    std::function<int(ASTNode*)> findFirstLine = [&](ASTNode* node) -> int {
                        if (!node) return functionLine;
                        
                        if (node->getSymbol() && node->getSymbol()->getLineNumber() > functionLine) {
                            return node->getSymbol()->getLineNumber();
                        }
                        
                        for (ASTNode* childNode : node->getChildren()) {
                            int childLine = findFirstLine(childNode);
                            if (childLine > functionLine) {
                                return childLine;
                            }
                        }
                        return functionLine;
                    };
                    
                    int bodyLine = findFirstLine(functionBody);
                    if (bodyLine > functionLine) {
                        functionLine = bodyLine;
                    }
                }
                
                // Check all return statements in the function body
                if (functionBody) {
                    errorCount += checkReturnStatementsInFunction(functionBody, expectedReturnType, functionName, functionLine);
                }
                break;
            }
            
            // Recursively check other nodes (but not their function bodies, as we handle those above)
            default:
                if (child->getType() != ASTNodeType::FUNCTION_DECLARATION) {
                    errorCount += checkFunctionReturnTypes(child);
                }
                break;
        }
    }

    return errorCount;
}

// Main semantic verification function
int semanticVerification(ASTNode* root) {
     // No AST to verify
    if (!root) return 0;

    // Collect all errors instead of stopping at the first one
    int declarationErrors = checkDeclarations(root);
    int undeclaredErrors = checkUndeclaredIdentifiers(root);
    int returnTypeErrors = checkFunctionReturnTypes(root);

    // Return total number of errors found
    return declarationErrors + undeclaredErrors + returnTypeErrors;
}
