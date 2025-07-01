// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// File main.cpp made by Nathan Guimaraes (334437)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "parser.tab.hpp"
#include "symbol.hpp"
#include "ast.hpp"
#include "verifications.hpp"
#include "tac.hpp"

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
    
    //fprintf(stderr, "\n======== AST Structure ========\n");
    //printAST(0, root);

    std::ofstream outFile(argv[2]);
    if (!outFile.is_open()) {
        fprintf(stderr, "File %s not found.\n", argv[2]);
        exit(2); // Exit code 2 for file not found
    }

    decompileAST(outFile, root, 0);
    outFile.close();
    fprintf(stderr, "\nDecompiled code can be found in file \"%s\".\n\n", argv[2]);

    int semanticErrors = semanticVerification(root);
    
    if (semanticErrors > 0) {
        fprintf(stderr, "Semantic analysis completed with %d error(s).\n", semanticErrors);
        exit(4); // Exit code 4 for semantic error 
    } else {
        fprintf(stderr, "Semantic analysis completed successfully - no errors found.\n");
    }

    fprintf(stderr, "\n====================  TAC Generation ====================\n");
    initTAC();
    TAC* tacCode = generateCode(root);
    if (tacCode) {
        tacPrint(tacCode);
        fprintf(stderr, "TAC generation completed successfully.\n");
    } else {
        fprintf(stderr, "TAC generation failed.\n");
    }

    return 0;
}