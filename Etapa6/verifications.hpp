// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File verifications.hpp made by Nathan Guimaraes (334437)

#include "ast.hpp"
#include "symbol.hpp"
#include <set>

// Public interface function, returns the number of semantic errors found
int semanticVerification(ASTNode* root);