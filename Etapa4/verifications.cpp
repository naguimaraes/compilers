// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File verifications.cpp made by Nathan Guimaraes (334437)

#include "verifications.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <functional>
#include <vector> 
#include <string> 
#include <map>

// Global map to store function declarations by name for parameter checking
std::map<std::string, ASTNode*> functionDeclarations; 

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
        case ASTNodeType::CHAR: // Added CHAR
            return dataType::CHAR;
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
                
                if (tokenType == LIT_INT) {
                    return dataType::INT;
                } else if (tokenType == LIT_REAL) {
                    return dataType::REAL;
                } else if (tokenType == LIT_CHAR) {
                    return dataType::CHAR;
                } else if (tokenType == TK_IDENTIFIER) {
                    // Check if the identifier is appropriate for scalar use
                    if (symbol->getIdentifierType() == identifierType::VECTOR ||
                        symbol->getIdentifierType() == identifierType::FUNCTION) {
                        return dataType::VOID; // Invalid for scalar operations
                    }
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
                
                if (leftType == dataType::REAL || rightType == dataType::REAL) {
                    return dataType::REAL;
                } else if (leftType == dataType::INT || rightType == dataType::INT) {
                    return dataType::INT;
                } else if (leftType == dataType::CHAR || rightType == dataType::CHAR) { // CHAR promotion if involved with BYTE
                    return dataType::CHAR; 
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
            return dataType::BOOLEAN; // Todas as operações relacionais e lógicas retornam booleano
        }

        case ASTNodeType::FUNCTION_CALL:
        {
            if (!expression->getChildren().empty() && 
                expression->getChildren()[0] && 
                expression->getChildren()[0]->getSymbol()) {
                return expression->getChildren()[0]->getSymbol()->getDataType(); // Return type of the function
            }
            break;
        }
        case ASTNodeType::VECTOR_ACCESS: // Added VECTOR_ACCESS
        {
            if (!expression->getChildren().empty() && 
                expression->getChildren()[0] && 
                expression->getChildren()[0]->getSymbol()) {
                // The type of a vector access is the type of the elements of the vector
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
    if (expected == actual) {
        return true;
    }
    // BYTE e INT são compatíveis
    if ((expected == dataType::BYTE && actual == dataType::INT) || 
        (expected == dataType::INT && actual == dataType::BYTE)) {
        return true;
    }
    // INT e CHAR são compatíveis
    if ((expected == dataType::INT && actual == dataType::CHAR) || 
        (expected == dataType::CHAR && actual == dataType::INT)) {
        return true;
    }
    // Implicit conversion para REAL
    if (expected == dataType::REAL && (actual == dataType::BYTE || actual == dataType::INT || actual == dataType::CHAR)) {
        return true;
    }
    // BOOLEAN só é compatível com BOOLEAN
    if (expected == dataType::BOOLEAN && actual == dataType::BOOLEAN) {
        return true;
    }
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
        case dataType::BOOLEAN:
            return "boolean";
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

// Forward declarations for functions that will be defined posteriorly
// int checkUsageAndTypesRecursive(ASTNode* node, int& errorCount); // No longer needed here, defined before main users
void collectParameterTypes(ASTNode* paramListNode, std::vector<dataType>& paramTypes);
void getFunctionParameterTypes(ASTNode* funcDeclNode, std::vector<dataType>& outParamTypes);

// Recursive helper to collect parameter types from PARAMETERS_LIST
void collectParameterTypes(ASTNode* paramListNode, std::vector<dataType>& paramTypes) {
    if (!paramListNode || paramListNode->getType() != ASTNodeType::PARAMETERS_LIST) {
        return;
    }

    auto children = paramListNode->getChildren();
    if (children.size() < 2) return; // Expect at least type and symbol

    ASTNode* typeNode = nullptr;
    ASTNode* nextParamList = nullptr;

    for (ASTNode* child : children) {
        if (!child) continue;
        if (child->getType() == ASTNodeType::INT ||
            child->getType() == ASTNodeType::BYTE ||
            child->getType() == ASTNodeType::REAL ||
            child->getType() == ASTNodeType::CHAR) { 
            typeNode = child;
        } else if (child->getType() == ASTNodeType::PARAMETERS_LIST) {
            nextParamList = child;
        }
    }

    if (typeNode) {
        paramTypes.push_back(convertToDataType(typeNode->getType()));
    }

    if (nextParamList) {
        collectParameterTypes(nextParamList, paramTypes);
    }
}

// Helper function to get parameter types from a function declaration node
void getFunctionParameterTypes(ASTNode* funcDeclNode, std::vector<dataType>& outParamTypes) {
    if (!funcDeclNode || funcDeclNode->getChildren().size() <= 2) {
        return; 
    }
    ASTNode* formalParameters = funcDeclNode->getChildren()[2];
    if (!formalParameters || formalParameters->getType() != ASTNodeType::FORMAL_PARAMETERS) {
        return; 
    }

    ASTNode* currentParamList = nullptr;
    for (ASTNode* child : formalParameters->getChildren()) {
        if (child && child->getType() == ASTNodeType::PARAMETERS_LIST) {
            currentParamList = child;
            break;
        }
    }
    collectParameterTypes(currentParamList, outParamTypes);
}


// ############## Verification functions ##############

// If the vector is correctly initialized, this function will return true
bool checkCorrectVectorInitialization(ASTNode* vectorDeclaration) {
    ASTNodeType vectorTypeNode = vectorDeclaration->getChildren()[0]->getType(); 
    Symbol* vectorSymbol = vectorDeclaration->getChildren()[1]->getSymbol(); 
    ASTNode* sizeNode = vectorDeclaration->getChildren()[2]; 
    ASTNode* initializationNode = vectorDeclaration->getChildren().size() > 3 ? vectorDeclaration->getChildren()[3] : nullptr;

    if (!initializationNode) return true;

    dataType expectedType = convertToDataType(vectorTypeNode);
    
    int vectorSize = 0;
    if (sizeNode && sizeNode->getSymbol() && sizeNode->getSymbol()->getType() == LIT_INT) {
        vectorSize = std::stoi(sizeNode->getSymbol()->getLexeme());
    } else {
        fprintf(stderr, "Semantic error: Cannot determine vector size for initialization check (vector: %s).\n", vectorSymbol->getLexeme().c_str());
        return false;
    }

    int initCount = 0;
    ASTNode* currentInit = initializationNode;
    
    while (currentInit && currentInit->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
        if (currentInit->getChildren().empty()) {
            break;
        }
        
        ASTNode* literal = nullptr;
        ASTNode* nextInit = nullptr;
        
        for (ASTNode* child : currentInit->getChildren()) {
            if (child && child->getType() == ASTNodeType::LITERAL) {
                literal = child;
            } else if (child && child->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
                nextInit = child;
            }
        }
        
        if (literal) {
            initCount++;
            dataType literalType = getExpressionType(literal);
            
            if (!isTypeCompatible(expectedType, literalType)) {            
                fprintf(stderr, "Semantic error at line %d: Vector \"%s\" of type %s cannot be initialized with value of type %s.\n", 
                        literal->getSymbol()->getLineNumber(),
                        vectorSymbol->getLexeme().c_str(), 
                        dataTypeToString(expectedType),
                        dataTypeToString(literalType));
                return false;
            }
        }
        currentInit = nextInit;
    }

    if (initCount != vectorSize) {
        fprintf(stderr, "Semantic error at line %d: Vector \"%s\" declared with size %d but initialized with %d elements.\n", 
                vectorSymbol->getLineNumber(), vectorSymbol->getLexeme().c_str(), vectorSize, initCount);
        return false;
    }
    return true;
}

int checkRedeclaration(ASTNode* declaration, identifierType expectedIdentifierType, const char* typeName) {
    Symbol* symbol = declaration->getChildren()[1]->getSymbol(); 
    if(symbol == nullptr) return 0;

    ASTNodeType type = declaration->getChildren()[0]->getType(); 
    dataType dt = convertToDataType(type);
    if (dt == dataType::BOOLEAN) {
        int line = symbol->getLineNumber();
        fprintf(stderr, "Semantic error at line %d: Cannot declare variable of type boolean.\n", line);
        return 1;
    }

    if (symbol->getIdentifierType() == identifierType::UNDEFINED) {
        symbol->setIdentifierType(expectedIdentifierType);
        symbol->setDataType(convertToDataType(type));
        return 0;
    } else {
        int firstDeclarationLine = symbol->getLineNumber();
        int currentDeclarationLine = firstDeclarationLine; 
        
        // Try to get the line number from the identifier node of the current declaration
        if (declaration->getChildren().size() > 1 && 
            declaration->getChildren()[1] && 
            declaration->getChildren()[1]->getSymbol()) {
            currentDeclarationLine = declaration->getChildren()[1]->getSymbol()->getLineNumber();
        } 
        // Fallback or alternative: if the type node (first child) has a symbol with a line number
        // This might happen if the type itself is complex or has associated token info
        else if (declaration->getChildren().size() > 0 && 
                   declaration->getChildren()[0] && 
                   declaration->getChildren()[0]->getSymbol()) {
             currentDeclarationLine = declaration->getChildren()[0]->getSymbol()->getLineNumber();
        }
        
        fprintf(stderr, "Semantic error at line %d: %s \"%s\" was redeclared (first declared at line %d).\n", 
                currentDeclarationLine, typeName, symbol->getLexeme().c_str(), firstDeclarationLine);
        return 1;
    }
}

int checkParameters(ASTNode* functionDeclaration) {
    Symbol* functionSymbol = functionDeclaration->getChildren()[1]->getSymbol();
    if (!functionSymbol) return 0;

    int errorCount = 0;
    ASTNode* formalParameters = functionDeclaration->getChildren().size() > 2 ? functionDeclaration->getChildren()[2] : nullptr;

    if (!formalParameters || formalParameters->getType() != ASTNodeType::FORMAL_PARAMETERS) {
        return 0; 
    }

    ASTNode* currentParamList = nullptr;
    for (ASTNode* child : formalParameters->getChildren()) {
        if (child && child->getType() == ASTNodeType::PARAMETERS_LIST) {
            currentParamList = child;
            break;
        }
    }

    while (currentParamList && currentParamList->getType() == ASTNodeType::PARAMETERS_LIST) {
        auto children = currentParamList->getChildren();
        if (children.size() < 2) break;
        
        ASTNode* typeNode = nullptr;
        ASTNode* symbolNode = nullptr;
        ASTNode* nextParamList = nullptr;

        for (ASTNode* child : children) {
            if (!child) continue;
            
            if (child->getType() == ASTNodeType::INT || 
                child->getType() == ASTNodeType::BYTE || 
                child->getType() == ASTNodeType::REAL ||
                child->getType() == ASTNodeType::CHAR) { 
                typeNode = child;
            } else if (child->getType() == ASTNodeType::SYMBOL) {
                symbolNode = child;
            } else if (child->getType() == ASTNodeType::PARAMETERS_LIST) {
                nextParamList = child;
            }
        }
        
        if (typeNode && symbolNode && symbolNode->getSymbol()) {
            Symbol* paramSymbol = symbolNode->getSymbol();
            if (paramSymbol->getIdentifierType() != identifierType::UNDEFINED) {
                fprintf(stderr, "Semantic error at line %d: Parameter \"%s\" was redeclared.\n", 
                        paramSymbol->getLineNumber(), paramSymbol->getLexeme().c_str());
                errorCount++;
            } else {
                paramSymbol->setIdentifierType(identifierType::VARIABLE); // Parameters are like local variables
                paramSymbol->setDataType(convertToDataType(typeNode->getType()));
            }
        }
        currentParamList = nextParamList;
    }
    return errorCount;
}

int checkDeclarations(ASTNode* root) {
    int errorCount = 0;
    if (!root) return errorCount;

    for (ASTNode* child : root->getChildren()) {
        if (!child) continue;

        switch (child->getType()) {
            case ASTNodeType::VARIABLE_DECLARATION: 
                errorCount += checkRedeclaration(child, identifierType::VARIABLE, "Variable");
                break;
            case ASTNodeType::VECTOR_DECLARATION: 
                errorCount += checkRedeclaration(child, identifierType::VECTOR, "Vector");
                if (errorCount == 0) { // Only check initialization if declaration was fine
                    if (!checkCorrectVectorInitialization(child)) errorCount++;
                }
                break;
            case ASTNodeType::FUNCTION_DECLARATION:
                errorCount += checkRedeclaration(child, identifierType::FUNCTION, "Function");
                if (errorCount == 0) { // Only check params if declaration was fine
                    errorCount += checkParameters(child); 
                }
                // Recursively check declarations within the function body (if any, though not typical for this structure)
                if (child->getChildren().size() > 3 && child->getChildren()[3]) { // Assuming body is 4th child
                     errorCount += checkDeclarations(child->getChildren()[3]);
                }
                break;
            default:
                errorCount += checkDeclarations(child); // Recurse for other constructs like DECLARATION_LIST
                break;
        }
    }
    return errorCount;
}

int checkUndeclaredIdentifiersRecursive(ASTNode* root) { // Renamed to avoid conflict
    int errorCount = 0;
    if (!root) return errorCount;
    
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; 

        if(child->getType() == ASTNodeType::SYMBOL) {
            // Undeclared check is tricky here. A symbol might be a type name in a declaration,
            // or a function name in a call, or a variable. 
            // This check is better done in context (e.g., in checkUsageAndTypes)
            // For now, we assume this is for identifiers used in expressions/assignments.
            // Also, function names are SYMBOLs but their declaration is handled by checkDeclarations.
            Symbol* sym = child->getSymbol();
            if (sym && sym->getIdentifierType() == identifierType::UNDEFINED && 
                sym->getDataType() == dataType::UNDEFINED) { // Check both to be sure
                // Avoid flagging function names that are being declared or called if already handled
                // This basic check might be too broad. Contextual checks are better.
                // fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is undeclared.\n", 
                //         sym->getLineNumber(), sym->getLexeme().c_str());
                // errorCount++;
            }
        }
        errorCount += checkUndeclaredIdentifiersRecursive(child);
    }
    return errorCount;
}

int checkReturnStatementsInFunction(ASTNode* functionBody, dataType expectedReturnType, const std::string& functionName, int functionLine) {
    if (!functionBody) return 0;
    int errorCount = 0;

    for (ASTNode* child : functionBody->getChildren()) {
        if (!child) continue;
        switch (child->getType()) {
            case ASTNodeType::RETURN: {
                dataType actualReturnType = dataType::VOID;
                int returnLine = functionLine; // Default to function line, try to get more specific

                if (!child->getChildren().empty() && child->getChildren()[0]) {
                    ASTNode* returnExpression = child->getChildren()[0];
                    actualReturnType = getExpressionType(returnExpression);
                    if (returnExpression->getSymbol()) returnLine = returnExpression->getSymbol()->getLineNumber();
                    else if (!returnExpression->getChildren().empty() && returnExpression->getChildren()[0]->getSymbol()) {
                        returnLine = returnExpression->getChildren()[0]->getSymbol()->getLineNumber();
                    }
                } else {
                    // void return, actualReturnType remains VOID
                    // Try to get line from RETURN keyword itself if possible (not directly available from ASTNode usually)
                }
                
                if (!isTypeCompatible(expectedReturnType, actualReturnType)) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" declared as %s but returns %s.\n",
                            returnLine, functionName.c_str(),
                            dataTypeToString(expectedReturnType), dataTypeToString(actualReturnType));
                    errorCount++;
                }
                break;
            }
            case ASTNodeType::COMMAND_LIST:
            case ASTNodeType::IF:
            case ASTNodeType::ELSE: // Check inside else too
            case ASTNodeType::WHILE_DO:
            case ASTNodeType::DO_WHILE:
                errorCount += checkReturnStatementsInFunction(child, expectedReturnType, functionName, functionLine);
                break;
            default:
                // For other node types that might contain commands or blocks, recursively check their children.
                // This ensures we find returns in nested structures.
                if (!child->getChildren().empty()){
                    errorCount += checkReturnStatementsInFunction(child, expectedReturnType, functionName, functionLine);
                }
                break;
        }
    }
    return errorCount;
}

int checkFunctionReturnTypes(ASTNode* root) {
    if (!root) return 0;
    int errorCount = 0;

    for (ASTNode* child : root->getChildren()) {
        if (!child) continue;

        if (child->getType() == ASTNodeType::FUNCTION_DECLARATION) {
            // Ensure children vector is not empty and has enough elements before accessing
            if (child->getChildren().empty() || child->getChildren().size() < 2) continue; 
            
            ASTNode* returnTypeNode = child->getChildren()[0]; 
            ASTNode* functionNameNode = child->getChildren()[1]; 
            // Check if body exists (optional parameters might mean fewer children)
            ASTNode* functionBody = child->getChildren().size() > 3 ? child->getChildren()[3] : nullptr; 
            
            if (!returnTypeNode || !functionNameNode || !functionNameNode->getSymbol()) continue;
            
            dataType expectedReturnType = convertToDataType(returnTypeNode->getType());
            std::string functionName = functionNameNode->getSymbol()->getLexeme();
            int functionLine = functionNameNode->getSymbol()->getLineNumber(); 
            
            if (functionBody) {
                errorCount += checkReturnStatementsInFunction(functionBody, expectedReturnType, functionName, functionLine);
            }
        } else {
             // Recursively check other nodes but not function declarations again (handled above)
            errorCount += checkFunctionReturnTypes(child);
        }
    }
    return errorCount;
}

// New function to check usage of identifiers and types in expressions/assignments
int checkUsageAndTypesRecursive(ASTNode* node, int& errorCount, int contextLine = 0) { // errorCount passed by reference, contextLine for line propagation
    if (!node) {
        return 0; 
    }
    int currentErrors = 0; 

    // Skip checking assignments that are part of declarations
    // Declarations are already handled by checkDeclarations
    if (node->getType() == ASTNodeType::VARIABLE_DECLARATION ||
        node->getType() == ASTNodeType::VECTOR_DECLARATION) {
        // Skip processing the content of variable/vector declarations entirely
        // as they are handled by checkDeclarations
        return 0;
    }
    
    // For function declarations, we need to check the body but not the declaration itself
    if (node->getType() == ASTNodeType::FUNCTION_DECLARATION) {
        // Check the function body (4th child if it exists)
        if (node->getChildren().size() > 3 && node->getChildren()[3]) {
            checkUsageAndTypesRecursive(node->getChildren()[3], errorCount, contextLine);
        }
        return 0; // Don't process other children of function declaration
    }

    switch (node->getType()) {
        case ASTNodeType::ASSIGNMENT: {
            if (node->getChildren().size() < 2) break;
            ASTNode* lhs = node->getChildren()[0];
            ASTNode* rhs = node->getChildren()[1];
            if (!lhs || !rhs) break;

            dataType rhsType = getExpressionType(rhs);
            dataType lhsExpectedType = dataType::VOID;
            int line = contextLine; // Start with context line
            if (line == 0) {
                // Fallback to finding line from symbols
                if (lhs->getSymbol()) line = lhs->getSymbol()->getLineNumber();
                else if (!lhs->getChildren().empty() && lhs->getChildren()[0]->getSymbol()) line = lhs->getChildren()[0]->getSymbol()->getLineNumber();
                else if (rhs->getSymbol()) line = rhs->getSymbol()->getLineNumber();
                else if (!rhs->getChildren().empty() && rhs->getChildren()[0]->getSymbol()) line = rhs->getChildren()[0]->getSymbol()->getLineNumber();
            }


            if (lhs->getType() == ASTNodeType::SYMBOL) {
                Symbol* lhsSymbol = lhs->getSymbol();
                if (!lhsSymbol) break;
                if(line == 0) line = lhsSymbol->getLineNumber();

                if (lhsSymbol->getIdentifierType() == identifierType::UNDEFINED) {
                     fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" not declared before assignment.\n",
                            line, lhsSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (lhsSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Cannot assign to function \"%s\".\n",
                            line, lhsSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (lhsSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Cannot assign to vector \"%s\" as a whole. Assign to an element e.g., vec[index].\n",
                            line, lhsSymbol->getLexeme().c_str());
                    currentErrors++;
                } else { // VARIABLE
                    lhsExpectedType = lhsSymbol->getDataType();
                }
            } else if (lhs->getType() == ASTNodeType::VECTOR_ACCESS) {
                if (lhs->getChildren().size() < 2) break;
                ASTNode* vecSymNode = lhs->getChildren()[0];
                ASTNode* indexNode = lhs->getChildren()[1];
                if (!vecSymNode || !vecSymNode->getSymbol() || !indexNode) break;
                
                Symbol* vecSymbol = vecSymNode->getSymbol();
                if(line == 0) line = vecSymbol->getLineNumber();

                if (vecSymbol->getIdentifierType() == identifierType::UNDEFINED) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" not declared before use in assignment.\n",
                            line, vecSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (vecSymbol->getIdentifierType() != identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is not a vector and cannot be indexed for assignment.\n",
                            line, vecSymbol->getLexeme().c_str());
                    currentErrors++;
                } else {
                    lhsExpectedType = vecSymbol->getDataType(); 
                }

                dataType indexType = getExpressionType(indexNode);
                int indexLine = indexNode->getSymbol() ? indexNode->getSymbol()->getLineNumber() : line;
                if (indexType == dataType::REAL || indexType == dataType::VOID ) { 
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" index cannot be of type %s.\n",
                            indexLine, vecSymbol->getLexeme().c_str(), dataTypeToString(indexType));
                    currentErrors++;
                }
            } else {
                 fprintf(stderr, "Semantic error at line %d: Left-hand side of assignment is not a valid l-value.\n", line);
                currentErrors++;
            }

            if (lhsExpectedType != dataType::VOID && rhsType != dataType::VOID) {
                if (!isTypeCompatible(lhsExpectedType, rhsType)) {
                    fprintf(stderr, "Semantic error at line %d: Type mismatch in assignment. Cannot assign %s to %s.\n",
                            line, dataTypeToString(rhsType), dataTypeToString(lhsExpectedType));
                    currentErrors++;
                }
            } else if (rhsType == dataType::VOID && lhsExpectedType != dataType::VOID && lhsExpectedType != dataType::UNDEFINED) {
                 fprintf(stderr, "Semantic error at line %d: Cannot assign void expression to %s.\n",
                            line, dataTypeToString(lhsExpectedType));
                    currentErrors++;
            }
            break;
        }
        case ASTNodeType::VECTOR_ACCESS: { 
            if (node->getChildren().size() < 2) break;
            ASTNode* vecSymNode = node->getChildren()[0];
            ASTNode* indexNode = node->getChildren()[1];
            if (!vecSymNode || !vecSymNode->getSymbol() || !indexNode) break;

            Symbol* vecSymbol = vecSymNode->getSymbol();
            int line = vecSymbol->getLineNumber();

            if (vecSymbol->getIdentifierType() == identifierType::UNDEFINED) {
                 fprintf(stderr, "Semantic error at line %d: Vector \"%s\" not declared before use.\n",
                        line, vecSymbol->getLexeme().c_str());
                currentErrors++;
            } else if (vecSymbol->getIdentifierType() != identifierType::VECTOR) {
                fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is not a vector and cannot be indexed.\n",
                        line, vecSymbol->getLexeme().c_str());
                currentErrors++;
            }

            dataType indexType = getExpressionType(indexNode);
            int indexLine = indexNode->getSymbol() ? indexNode->getSymbol()->getLineNumber() : line;
            if (indexType == dataType::REAL || indexType == dataType::VOID) {
                 fprintf(stderr, "Semantic error at line %d: Vector \"%s\" index cannot be of type %s.\n",
                        indexLine, vecSymbol->getLexeme().c_str(), dataTypeToString(indexType));
                currentErrors++;
            }
            break;
        }
        case ASTNodeType::FUNCTION_CALL: {
            if (node->getChildren().empty()) break;
            ASTNode* funcSymNode = node->getChildren()[0];
            if (!funcSymNode || !funcSymNode->getSymbol()) break;

            Symbol* funcSymbol = funcSymNode->getSymbol();
            int line = funcSymbol->getLineNumber();

            if (funcSymbol->getIdentifierType() == identifierType::UNDEFINED) {
                fprintf(stderr, "Semantic error at line %d: Function \"%s\" not declared before call.\n",
                        line, funcSymbol->getLexeme().c_str());
                currentErrors++;
            } else if (funcSymbol->getIdentifierType() != identifierType::FUNCTION) {
                fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is not a function and cannot be called.\n",
                        line, funcSymbol->getLexeme().c_str());
                currentErrors++;
            } else {
                std::vector<dataType> expectedParamTypes;
                // Find the function declaration in our global map
                auto it = functionDeclarations.find(funcSymbol->getLexeme());
                if (it != functionDeclarations.end()) {
                    ASTNode* funcDeclNode = it->second;
                    getFunctionParameterTypes(funcDeclNode, expectedParamTypes);
                }

                // Collect actual arguments from the AST structure
                std::vector<ASTNode*> actualArgs;
                if (node->getChildren().size() > 1) {
                    // Second child should be ARGUMENTS_LIST
                    ASTNode* argsListNode = node->getChildren()[1];
                    if (argsListNode && argsListNode->getType() == ASTNodeType::ARGUMENTS_LIST) {
                        // ARGUMENTS_LIST contains EXPRESSION_LIST as child
                        if (!argsListNode->getChildren().empty()) {
                            ASTNode* exprListNode = argsListNode->getChildren()[0];
                            if (exprListNode) {
                                // Collect expressions from EXPRESSION_LIST recursively
                                std::function<void(ASTNode*)> collectExpressions = [&](ASTNode* node) {
                                    if (!node) return;
                                    if (node->getType() == ASTNodeType::EXPRESSION_LIST) {
                                        // EXPRESSION_LIST has expression as first child, then optional EXPRESSION_LIST as second
                                        if (!node->getChildren().empty()) {
                                            actualArgs.push_back(node->getChildren()[0]); // Add the expression
                                            if (node->getChildren().size() > 1) {
                                                collectExpressions(node->getChildren()[1]); // Recurse on next EXPRESSION_LIST
                                            }
                                        }
                                    } else {
                                        // Single expression (not wrapped in EXPRESSION_LIST)
                                        actualArgs.push_back(node);
                                    }
                                };
                                collectExpressions(exprListNode);
                            }
                        }
                    }
                }
                
                size_t numArgsProvided = actualArgs.size();
                size_t numParamsExpected = expectedParamTypes.size(); 

                // Check argument count
                if (numArgsProvided != numParamsExpected) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" expects %zu arguments, but %zu were provided.\n",
                            line, funcSymbol->getLexeme().c_str(), numParamsExpected, numArgsProvided);
                    currentErrors++;
                } else {
                    // Check argument types
                    for (size_t i = 0; i < numArgsProvided; ++i) {
                        ASTNode* argNode = actualArgs[i];
                        dataType argType = getExpressionType(argNode);
                        int argLine = line; // Default to function call line
                        
                        // Try to get more specific line number from argument
                        if (argNode->getSymbol()) {
                            argLine = argNode->getSymbol()->getLineNumber();
                        } else if (!argNode->getChildren().empty() && argNode->getChildren()[0]->getSymbol()) {
                            argLine = argNode->getChildren()[0]->getSymbol()->getLineNumber();
                        }
                        
                        if (!isTypeCompatible(expectedParamTypes[i], argType)) {
                            fprintf(stderr, "Semantic error at line %d: Argument %zu of function \"%s\" type mismatch. Expected %s, got %s.\n",
                                    argLine, i + 1, funcSymbol->getLexeme().c_str(),
                                    dataTypeToString(expectedParamTypes[i]), dataTypeToString(argType));
                            currentErrors++;
                        }
                    }
                }
            }
            break;
        }
        case ASTNodeType::SYMBOL: { 
            Symbol* sym = node->getSymbol();
            if (!sym) break;
            if (sym->getIdentifierType() == identifierType::UNDEFINED) {
                 fprintf(stderr, "Semantic error at line %d: Identifier \"%s\" is undeclared.\n", 
                        sym->getLineNumber(), sym->getLexeme().c_str());
                currentErrors++;
            }
            // Further checks for SYMBOL if it's used in a context where it shouldn't be
            // e.g. `if (myFunction)` where `myFunction` is a function symbol.
            // `getExpressionType` for a function symbol returns its return type, which might be valid in an expression.
            // The nature check (scalar vs function/vector) is implicitly handled by how `getExpressionType` works
            // for SYMBOL (it returns the symbol's data type). If a function or vector is used where a scalar is expected,
            // and its data type (e.g. VOID for a function that doesn't return, or if vectors had a special 'array' type)
            // is incompatible, then type checking rules in operations (ADD, SUB etc.) or assignments would catch it.
            break;
        }
        case ASTNodeType::MODULO: {
            if (node->getChildren().size() < 2) break;
            ASTNode* left = node->getChildren()[0];
            ASTNode* right = node->getChildren()[1];
            if (!left || !right) break;

            dataType leftType = getExpressionType(left);
            dataType rightType = getExpressionType(right);
            int line = node->getLineNumber(); // Use line number from AST node
            if (line == 0) line = contextLine; // Fallback to context
            if (line == 0) {
                // Further fallback to finding line from symbols
                if (left->getSymbol()) line = left->getSymbol()->getLineNumber();
                else if (!left->getChildren().empty() && left->getChildren()[0]->getSymbol()) line = left->getChildren()[0]->getSymbol()->getLineNumber();
                else if (right->getSymbol()) line = right->getSymbol()->getLineNumber();
                else if (!right->getChildren().empty() && right->getChildren()[0]->getSymbol()) line = right->getChildren()[0]->getSymbol()->getLineNumber();
            }

            // Check for invalid operand types for modulo
            if (leftType == dataType::REAL || rightType == dataType::REAL || leftType == dataType::VOID || rightType == dataType::VOID) {
                fprintf(stderr, "Semantic error at line %d: Operands of modulo operator %% cannot be of type REAL or VOID.\n", line);
                currentErrors++;
            }
            
            // Check if operands are vectors or functions used as scalars
            if (left->getType() == ASTNodeType::SYMBOL && left->getSymbol()) {
                Symbol* leftSymbol = left->getSymbol();
                if (leftSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in modulo operation. Use vector indexing instead.\n", 
                            line, leftSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (leftSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in modulo operation.\n", 
                            line, leftSymbol->getLexeme().c_str());
                    currentErrors++;
                }
            }
            if (right->getType() == ASTNodeType::SYMBOL && right->getSymbol()) {
                Symbol* rightSymbol = right->getSymbol();
                if (rightSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in modulo operation. Use vector indexing instead.\n", 
                            line, rightSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (rightSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in modulo operation.\n", 
                            line, rightSymbol->getLexeme().c_str());
                    currentErrors++;
                }
            }
            break;
        }
        case ASTNodeType::ADD:
        case ASTNodeType::SUBTRACT:
        case ASTNodeType::MULTIPLY:
        case ASTNodeType::DIVIDE: {
            if (node->getChildren().size() < 2) break;
            ASTNode* left = node->getChildren()[0];
            ASTNode* right = node->getChildren()[1];
            if (!left || !right) break;

            int line = node->getLineNumber(); // Use line number from AST node
            if (line == 0) line = contextLine; // Fallback to context
            if (line == 0) {
                // Further fallback to finding line from symbols
                if (left->getSymbol()) line = left->getSymbol()->getLineNumber();
                else if (!left->getChildren().empty() && left->getChildren()[0]->getSymbol()) line = left->getChildren()[0]->getSymbol()->getLineNumber();
                else if (right->getSymbol()) line = right->getSymbol()->getLineNumber();
                else if (!right->getChildren().empty() && right->getChildren()[0]->getSymbol()) line = right->getChildren()[0]->getSymbol()->getLineNumber();
            }

            // Check if operands are vectors or functions used as scalars
            if (left->getType() == ASTNodeType::SYMBOL && left->getSymbol()) {
                Symbol* leftSymbol = left->getSymbol();
                if (leftSymbol->getIdentifierType() == identifierType::VECTOR) {
                    const char* opName = (node->getType() == ASTNodeType::ADD) ? "addition" :
                                        (node->getType() == ASTNodeType::SUBTRACT) ? "subtraction" :
                                        (node->getType() == ASTNodeType::MULTIPLY) ? "multiplication" : "division";
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in %s. Use vector indexing instead.\n", 
                            line, leftSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                } else if (leftSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    const char* opName = (node->getType() == ASTNodeType::ADD) ? "addition" :
                                        (node->getType() == ASTNodeType::SUBTRACT) ? "subtraction" :
                                        (node->getType() == ASTNodeType::MULTIPLY) ? "multiplication" : "division";
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in %s.\n", 
                            line, leftSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                }
            }
            if (right->getType() == ASTNodeType::SYMBOL && right->getSymbol()) {
                Symbol* rightSymbol = right->getSymbol();
                if (rightSymbol->getIdentifierType() == identifierType::VECTOR) {
                    const char* opName = (node->getType() == ASTNodeType::ADD) ? "addition" :
                                        (node->getType() == ASTNodeType::SUBTRACT) ? "subtraction" :
                                        (node->getType() == ASTNodeType::MULTIPLY) ? "multiplication" : "division";
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in %s. Use vector indexing instead.\n", 
                            line, rightSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                } else if (rightSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    const char* opName = (node->getType() == ASTNodeType::ADD) ? "addition" :
                                        (node->getType() == ASTNodeType::SUBTRACT) ? "subtraction" :
                                        (node->getType() == ASTNodeType::MULTIPLY) ? "multiplication" : "division";
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in %s.\n", 
                            line, rightSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                }
            }
            break;
        }
        case ASTNodeType::LESS_THAN:
        case ASTNodeType::GREATER_THAN:
        case ASTNodeType::LESS_EQUAL:
        case ASTNodeType::GREATER_EQUAL:
        case ASTNodeType::EQUAL:
        case ASTNodeType::DIFFERENT: {
            if (node->getChildren().size() < 2) break;
            ASTNode* left = node->getChildren()[0];
            ASTNode* right = node->getChildren()[1];
            if (!left || !right) break;

            int line = node->getLineNumber(); // Use line number from AST node
            if (line == 0) line = contextLine; // Fallback to context
            if (line == 0) {
                // Further fallback to finding line from symbols
                if (left->getSymbol()) line = left->getSymbol()->getLineNumber();
                else if (!left->getChildren().empty() && left->getChildren()[0]->getSymbol()) line = left->getChildren()[0]->getSymbol()->getLineNumber();
                else if (right->getSymbol()) line = right->getSymbol()->getLineNumber();
                else if (!right->getChildren().empty() && right->getChildren()[0]->getSymbol()) line = right->getChildren()[0]->getSymbol()->getLineNumber();
            }

            // Check if operands are vectors or functions used as scalars
            if (left->getType() == ASTNodeType::SYMBOL && left->getSymbol()) {
                Symbol* leftSymbol = left->getSymbol();
                if (leftSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in comparison operation. Use vector indexing instead.\n", 
                            line, leftSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (leftSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in comparison operation.\n", 
                            line, leftSymbol->getLexeme().c_str());
                    currentErrors++;
                }
            }
            if (right->getType() == ASTNodeType::SYMBOL && right->getSymbol()) {
                Symbol* rightSymbol = right->getSymbol();
                if (rightSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in comparison operation. Use vector indexing instead.\n", 
                            line, rightSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (rightSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in comparison operation.\n", 
                            line, rightSymbol->getLexeme().c_str());
                    currentErrors++;
                }
            }
            break;
        }
        case ASTNodeType::AND:
        case ASTNodeType::OR: {
            if (node->getChildren().size() < 2) break;
            ASTNode* left = node->getChildren()[0];
            ASTNode* right = node->getChildren()[1];
            if (!left || !right) break;

            int line = node->getLineNumber(); // Use line number from AST node
            if (line == 0) line = contextLine; // Fallback to context
            if (line == 0) {
                // Further fallback to finding line from symbols
                if (left->getSymbol()) line = left->getSymbol()->getLineNumber();
                else if (!left->getChildren().empty() && left->getChildren()[0]->getSymbol()) line = left->getChildren()[0]->getSymbol()->getLineNumber();
                else if (right->getSymbol()) line = right->getSymbol()->getLineNumber();
                else if (!right->getChildren().empty() && right->getChildren()[0]->getSymbol()) line = right->getChildren()[0]->getSymbol()->getLineNumber();
            }

            // Check if operands are vectors or functions used as scalars
            if (left->getType() == ASTNodeType::SYMBOL && left->getSymbol()) {
                Symbol* leftSymbol = left->getSymbol();
                if (leftSymbol->getIdentifierType() == identifierType::VECTOR) {
                    const char* opName = (node->getType() == ASTNodeType::AND) ? "logical AND" : "logical OR";
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in %s operation. Use vector indexing instead.\n", 
                            line, leftSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                } else if (leftSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    const char* opName = (node->getType() == ASTNodeType::AND) ? "logical AND" : "logical OR";
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in %s operation.\n", 
                            line, leftSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                }
            }
            if (right->getType() == ASTNodeType::SYMBOL && right->getSymbol()) {
                Symbol* rightSymbol = right->getSymbol();
                if (rightSymbol->getIdentifierType() == identifierType::VECTOR) {
                    const char* opName = (node->getType() == ASTNodeType::AND) ? "logical AND" : "logical OR";
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in %s operation. Use vector indexing instead.\n", 
                            line, rightSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                } else if (rightSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    const char* opName = (node->getType() == ASTNodeType::AND) ? "logical AND" : "logical OR";
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in %s operation.\n", 
                            line, rightSymbol->getLexeme().c_str(), opName);
                    currentErrors++;
                }
            }
            break;
        }
        case ASTNodeType::NOT: {
            if (node->getChildren().size() < 1) break;
            ASTNode* operand = node->getChildren()[0];
            if (!operand) break;

            int line = node->getLineNumber(); // Use line number from AST node
            if (line == 0) line = contextLine; // Fallback to context
            if (line == 0) {
                // Further fallback to finding line from symbols
                if (operand->getSymbol()) line = operand->getSymbol()->getLineNumber();
                else if (!operand->getChildren().empty() && operand->getChildren()[0]->getSymbol()) line = operand->getChildren()[0]->getSymbol()->getLineNumber();
            }

            // Check if operand is vector or function used as scalar
            if (operand->getType() == ASTNodeType::SYMBOL && operand->getSymbol()) {
                Symbol* operandSymbol = operand->getSymbol();
                if (operandSymbol->getIdentifierType() == identifierType::VECTOR) {
                    fprintf(stderr, "Semantic error at line %d: Vector \"%s\" cannot be used as scalar in logical NOT operation. Use vector indexing instead.\n", 
                            line, operandSymbol->getLexeme().c_str());
                    currentErrors++;
                } else if (operandSymbol->getIdentifierType() == identifierType::FUNCTION) {
                    fprintf(stderr, "Semantic error at line %d: Function \"%s\" cannot be used as scalar in logical NOT operation.\n", 
                            line, operandSymbol->getLexeme().c_str());
                    currentErrors++;
                }
            }
            break;
        }
        // For other binary/unary operations, getExpressionType handles type inference.
        // We need to ensure operands are valid for those operations if getExpressionType doesn't already.
        // For example, logical operators (AND, OR) should ideally operate on boolean (INT in this case).
        // Arithmetic operators should operate on numeric types.
        // getExpressionType implicitly handles type promotion, but not operand validity for all ops.

        default:
            break;
    }

    for (ASTNode* child : node->getChildren()) {
        if (child) { 
            // Try to find line number for context propagation
            int newContextLine = contextLine;
            if (newContextLine == 0) {
                // Look for line information in the current node or its direct children
                if (node->getSymbol()) {
                    newContextLine = node->getSymbol()->getLineNumber();
                } else {
                    // Look in children for symbols with line numbers
                    for (ASTNode* grandchild : node->getChildren()) {
                        if (grandchild && grandchild->getSymbol()) {
                            newContextLine = grandchild->getSymbol()->getLineNumber();
                            break;
                        }
                    }
                }
            }
            checkUsageAndTypesRecursive(child, errorCount, newContextLine); // Pass errorCount and context line
        }
    }
    errorCount += currentErrors; // Add errors from this node to the total
    return currentErrors; // Return only errors from this specific node and its direct processing
}

int checkUsageAndTypes(ASTNode* root) {
    int totalErrors = 0;
    checkUsageAndTypesRecursive(root, totalErrors);
    return totalErrors;
}

// Function to collect all function declarations in a first pass
void collectFunctionDeclarations(ASTNode* root) {
    if (!root) return;
    
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue;
        
        if (child->getType() == ASTNodeType::FUNCTION_DECLARATION) {
            if (child->getChildren().size() > 1 && child->getChildren()[1]->getSymbol()) {
                std::string funcName = child->getChildren()[1]->getSymbol()->getLexeme();
                functionDeclarations[funcName] = child;
            }
        }
        
        // Recursively collect from children
        collectFunctionDeclarations(child);
    }
}

// Main semantic verification function
int semanticVerification(ASTNode* root) {
    if (!root) return 0;

    // Clear any previous function declarations
    functionDeclarations.clear();
    
    // First pass: collect all function declarations
    collectFunctionDeclarations(root);

    int totalErrors = 0;
    totalErrors += checkDeclarations(root); // Handles redeclarations, vector init, function params
    // Undeclared identifiers are now checked more contextually within checkUsageAndTypesRecursive
    // totalErrors += checkUndeclaredIdentifiersRecursive(root); // This can be removed or refined
    totalErrors += checkUsageAndTypes(root); 
    totalErrors += checkFunctionReturnTypes(root);

    return totalErrors;
}
