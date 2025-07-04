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
#include "asm.hpp"

int yyparse();
extern char *yytext;

extern int getLineNumber();
extern void initMe();
extern FILE *yyin;
extern ASTNode* root;
string tokenName(int token);

int main(int argc, char **argv){
    if (argc < 7){
        fprintf(stderr, "Arguments missing.\n");
        fprintf(stderr, "Call: ./etapa6 [input file] [output symbol table] [output AST] [decompiled input file] [output TAC] [output assembly]\n");
        exit(1); // Exit code 1 for missing arguments
    }
    
    if (0 == (yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "Input file %s not found.\n", argv[1]);
        exit(2); // Exit code 2 for input file not found
    }

    // Syntax analysis and AST and symbol table generation
    initMe();
    yyparse(); // Exit code 3 for syntax error
    fprintf(stderr,"The file had %d lines in total.\n", getLineNumber());
    std::ofstream symbolFile(argv[2]);
    if (!symbolFile.is_open()) {
        fprintf(stderr, "File %s could not be opened for writing.\n", argv[2]);
        exit(2); // Exit code 2 for file not found
    }
    symbolFile.close();
    printSymbolTableToFile(argv[2]);
    fprintf(stderr, "- Symbol table saved to file \"%s\".\n", argv[2]);
    
    // Save AST structure to file
    std::ofstream astFile(argv[3]);
    if (!astFile.is_open()) {
        fprintf(stderr, "File %s could not be opened for writing.\n", argv[3]);
        exit(2); // Exit code 2 for file not found
    }
    astFile.close();
    printASTToFile(argv[3], root);
    fprintf(stderr, "- AST structure saved to file \"%s\".\n", argv[3]);
    
    // Save decompiled code to file
    std::ofstream outFile(argv[4]);
    if (!outFile.is_open()) {
        fprintf(stderr, "File %s could not be opened for writing.\n", argv[4]);
        exit(2); // Exit code 2 for file not found
    }
    decompileAST(outFile, root, 0);
    outFile.close();
    fprintf(stderr, "- Decompiled code saved to file \"%s\".\n", argv[4]);

    int semanticErrors = semanticVerification(root);
    
    if (semanticErrors > 0) {
        fprintf(stderr, "Semantic analysis completed with %d error(s). Compilation ended.\n", semanticErrors);
        exit(4); // Exit code 4 for semantic error 
    } else {
        fprintf(stderr, "Semantic analysis completed successfully - no errors found.\n");
    }

    initTAC();
    TAC* tacCode = generateTAC(root);
    if (tacCode) {
        // Save TAC to file
        std::ofstream tacFile(argv[5]);
        if (!tacFile.is_open()) {
            fprintf(stderr, "File %s could not be opened for writing.\n", argv[5]);
            exit(2); // Exit code 2 for file not found
        }
        tacFile.close();
        printTACToFile(argv[5], tacCode);
        fprintf(stderr, "- TAC list saved to file \"%s\".\n", argv[5]);
        
        // Generate assembly code
        std::ofstream asmFile(argv[6]);
        if (!asmFile.is_open()) {
            fprintf(stderr, "File %s could not be opened for writing.\n", argv[6]);
            exit(2); // Exit code 2 for file not found
        }
        asmFile.close();
        generateASM(tacCode, argv[6]);
        fprintf(stderr, "- Assembly code saved to file \"%s\".\n", argv[6]);
    } else {
        fprintf(stderr, "TAC generation failed.\n");
    }

    return 0;
}