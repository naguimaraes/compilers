#include "ast.hpp"
#include "symbol.hpp"
#include <set>

bool semanticVerification(ASTNode* root);

bool redeclaration(ASTNode* root, std::set<std::string>& declaredVariables);