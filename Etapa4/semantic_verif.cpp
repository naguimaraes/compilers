#include "semantic_verif.hpp"
#include "parser.tab.hpp"

bool semanticVerification(ASTNode* root) {
    if (!root) 
        return false; // No AST to verify

    std::set<std::string> declaredVariables;
    if(redeclaration(root, declaredVariables))
        return false; // Return false if a redeclaration is found



    return true; // Return true if no semantic errors are found
}

bool redeclaration(ASTNode* root, std::set<std::string>& declaredVariables) {
    for (ASTNode* child : root->getChildren()) {
        if (!child) continue; // Skip null children

        switch (child->getType()) {
            case ASTNodeType::VARIABLE_DECLARATION: 
            {
                Symbol* symbol = child->getChildren()[1]->getSymbol(); // The second child is the symbol

                if(symbol == nullptr) continue;

                std::string lexeme = symbol->getLex();
                if (declaredVariables.find(lexeme) != declaredVariables.end()) {
                    fprintf(stderr, "Semantic error: Variable \"%s\" was redeclared.\n", lexeme.c_str());
                    return true; // Redeclared variable found
                } else {
                    declaredVariables.insert(lexeme);
                }
            }
            break;

            case ASTNodeType::VECTOR_DECLARATION: 
            {
                Symbol* symbol = child->getChildren()[1]->getSymbol(); // The second child is the symbol

                if(symbol == nullptr) continue;

                std::string lexeme = symbol->getLex();
                if (declaredVariables.find(lexeme) != declaredVariables.end()) {
                    fprintf(stderr, "Semantic error: Vector \"%s\" was redeclared.\n", lexeme.c_str());
                    return true; // Redeclared vector found
                } else {
                    declaredVariables.insert(lexeme);
                }
            }
            break;
            
            case ASTNodeType::FUNCTION_DECLARATION: 
            {
                Symbol* symbol = child->getChildren()[1]->getSymbol(); // The second child is the symbol

                if(symbol == nullptr) continue;

                std::string lexeme = symbol->getLex();
                if (declaredVariables.find(lexeme) != declaredVariables.end()) {
                    fprintf(stderr, "Semantic error: Function \"%s\" was redeclared.\n", lexeme.c_str());
                    return true; // Redeclared function found
                } else {
                    declaredVariables.insert(lexeme);
                }
            }
            break;

            // Recursively check children with the same set
            default:
                if (redeclaration(child, declaredVariables)) {
                    return true; // Propagate the error up
                }
            break;
        }
    }
    
    return false; // No redeclared variable found in this subtree
}