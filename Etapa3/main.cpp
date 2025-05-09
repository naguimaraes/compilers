// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "parser.tab.hpp"
#include "symbol.hpp"
#include "ast.hpp"

int yyparse();
extern char *yytext;

extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
string tokenName(int token);
extern ASTNode* root;

int main(int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, "call: ./etapa3 input.txt output.txt\n");
        exit(1);
    }
    
    if (0 == (yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "Cannot open input file %s... \n", argv[1]);
        exit(2);
    }

    initMe();

    yyparse();

    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());

    printSymbolTable();
    
    fprintf(stderr, "\n======== AST Structure ========\n");
    printAST(0, root);

    std::ofstream outFile(argv[2]);
    if (!outFile.is_open()) {
        fprintf(stderr, "Cannot open output file %s... \n", argv[2]);
        exit(2);
    }

    fprintf(stderr, "\n======== Decompiling Code to file: %s ========\n", argv[2]);
    decompileAST(outFile, root, 0);
    outFile.close();
    fprintf(stderr, "Decompilation completed successfully.\n");
    return 0;
}