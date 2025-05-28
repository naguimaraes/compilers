// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "parser.tab.hpp"
#include "symbol.hpp"
#include "ast.hpp"
#include "semantic_verif.hpp"

int yyparse();
extern char *yytext;

extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
extern ASTNode* root;
string tokenName(int token);

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, "Arguments missing. Call: ./etapa4 input.txt output.txt\n");
        exit(1); // Exit code 1 for missing arguments
    }
    
    if (0 == (yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "File %s not found.\n", argv[1]);
        exit(2); // Exit code 2 for file not found
    }

    initMe();

    yyparse(); // Exit code 3 for syntax error

    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());

    //printSymbolTable();
    
    fprintf(stderr, "\n======== AST Structure ========\n");
    printAST(0, root);

    std::ofstream outFile(argv[2]);
    if (!outFile.is_open()) {
        fprintf(stderr, "File %s not found.\n", argv[2]);
        exit(2); // Exit code 2 for file not found
    }

    decompileAST(outFile, root, 0);
    outFile.close();
    fprintf(stderr, "Decompiled code can be found in file \"%s\".\n", argv[2]);

    if (!semanticVerification(root)) {
        fprintf(stderr, "Semantic verification failed.\n");
        exit(4); // Exit code 4 for semantic error 
    }

    return 0;
}